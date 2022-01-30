#include "parser/ast.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Context {
    int field_offset;
    std::map<std::string, int> headers;
    int var_id;
};

class JsonEmitter {
public:
    static json emit(const ast::RP4AST *tree) {
        Context ctx;
        return visit(tree, ctx);
    }

private:
    static json visit(const ast::TreeNode *node, Context &ctx) {
        json out;
        if (node == nullptr) {
            out = nullptr;
        } else if (auto x = dynamic_cast<const ast::RP4AST *>(node)) {
            ctx.var_id = 0;
            for (auto &hdr: x->headers_def) {
                ctx.headers[hdr.name] = ctx.var_id;
                ctx.var_id++;
            }
            out["headers"] = json::array();
            for (auto &hdr : x->headers_def) {
                auto jhdr = visit(&hdr, ctx);
                out["headers"].push_back(jhdr);
            }
            out["structs"] = json::array();
            for (auto &st: x->structs_def) {
                auto jstruct = visit(&st, ctx);
                out["structs"].push_back(jstruct);
            }
            out["actions"] = json::array();
            for (auto &act: x->actions_def) {
                auto jact = visit(&act, ctx);
                out["actions"].push_back(jact);
            }
            out["tables"] = json::array();
            for (auto &tbl: x->tables_def) {
                auto jtbl = visit(&tbl, ctx);
                out["tables"].push_back(jtbl);
            }
            out["controls"] = json::array();
            for (auto &ctrl: x->controls_def) {
                auto jctrl = visit(&ctrl, ctx);
                out["controls"].push_back(jctrl);
            }
        } else if (auto x = dynamic_cast<const ast::HeaderDef *> (node)) {
            out["name"] = x->name;
            out["id"] = ctx.headers[x->name];
            out["fields"] = json::array();
            ctx.field_offset = 0;
            for (auto &field: x->fields) {
                auto jfield = visit(&field, ctx);
                out["fields"].push_back(jfield);
            }
            ctx.field_offset = 0;
            out["parser"] = visit(&x->next_table, ctx);
        } else if (auto x = dynamic_cast<const ast::FieldDef *> (node)) {
            std::string type;
            int len;
            int size;
            if (auto bit_type = dynamic_cast<const ast::BitType *> (x->type.get())) {
                type = "bit";
                len = bit_type->len;
                size = -1;
            } else if (auto reg_type = dynamic_cast<const ast::RegisterType *>(x->type.get())) {
                type = "register";
                auto bit_type = dynamic_cast<const ast::BitType *> (reg_type->type.get());
                assert(bit_type);
                len = bit_type->len;
                size = reg_type->size;
            }
            out["name"] = x->name;
            out["type"] = type;
            out["length"] = len;
            out["offset"] = ctx.field_offset;
            out["size"] = size;
            ctx.field_offset += len;
        } else if (auto x = dynamic_cast<const ast::NextTable *>(node)) {
            out["field"] = x->tag_name;
            out["transition"] = json::array();
            for (auto &e: x->table) {
                auto tag = e.key;
                if (tag == -1) {
                    continue;
                }
                auto &hdr_name = e.val;
                json jentry = {
                        {"tag",            tag},
                        {"next_header",    hdr_name},
                        {"next_header_id", ctx.headers[hdr_name]}
                };
                out["transition"].push_back(jentry);
            }
        } else if (auto x = dynamic_cast<const ast::StructDef * > (node)) {
            out["name"] = x->type_name;
            out["id"] = ctx.var_id++;
            out["alias"] = x->var_name;
            out["fields"] = json::array();
            ctx.field_offset = 0;
            for (auto &field: x->fields) {
                auto jfield = visit(&field, ctx);
                out["fields"].push_back(jfield);
            }
            ctx.field_offset = 0;
        } else if (auto x = dynamic_cast<const ast::ActionDef *>(node)) {
            out["name"] = x->name;
            out["args"] = json::array();
            for (auto &field: x->args) {
                auto jfield = visit(&field, ctx);
                out["args"].push_back(jfield);
            }
            out["ops"] = json::array();
            for (auto &op: x->stmts) {
                auto jop = visit(op.get(), ctx);
                out["ops"].push_back(jop);
            }
        } else if (auto x = dynamic_cast<const ast::GotoStmt *> (node)) {
            out["op"] = "goto";
            out["tag"] = x->tag;
        } else if (auto x = dynamic_cast<const ast::ExprStmt *>(node)) {
            auto call_expr = dynamic_cast<const ast::CallExpr *>(x->expr.get());
            assert(call_expr);
            auto caller = dynamic_cast<const ast::MemberOf *> (call_expr->caller.get());
            out["op"] = caller->name;
            out["args"] = json::array();
            for (auto &arg: call_expr->params) {
                auto jarg = visit(arg.get(), ctx);
                out["args"].push_back(jarg);
            }
        } else if (auto x = dynamic_cast<const ast::IntLit *>(node)) {
            out["type"] = "int";
            out["val"] = x->val;
        } else if (auto x = dynamic_cast<const ast::BinaryExpr *>(node)) {
            out["op"] = ast::to_string(x->op);
            out["lhs"] = visit(x->lhs.get(), ctx);
            out["rhs"] = visit(x->rhs.get(), ctx);
        } else if (auto x = dynamic_cast<const ast::MemberOf *>(node)) {
            out["type"] = "member_of";
            out["recv"] = visit(x->recv.get(), ctx);
            out["name"] = x->name;
        } else if (auto x = dynamic_cast<const ast::AssignStmt *>(node)) {
            out["type"] = "assign";
            out["lhs"] = visit(x->lhs.get(), ctx);
            out["rhs"] = visit(x->rhs.get(), ctx);
        } else if (auto x = dynamic_cast<const ast::TableDef *> (node)) {
            out["name"] = x->name;
            out["size"] = x->size;
            out["keys"] = json::array();
            for (auto &field: x->match_keys) {
                out["keys"].push_back(visit(&field, ctx));
            }
        } else if (auto x = dynamic_cast<const ast::MatchKey *>(node)) {
            out["name"] = visit(x->key.get(), ctx);
            out["match_type"] = ast::to_string(x->match_type);
        } else if (auto x = dynamic_cast<const ast::ControlDef *>(node)) {
            out["name"] = x->name;
            out["stages"] = json::array();
            for (auto &stage: x->stages) {
                out["stages"].push_back(visit(&stage, ctx));
            }
        } else if (auto x = dynamic_cast<const ast::StageDef *>(node)) {
            out["name"] = x->name;
            out["parser"] = x->parser;
            out["matcher"] = visit(x->matcher.get(), ctx);
            out["executor"] = json::array();
            for (auto &act: x->executor) {
                json jact = {{act.first, act.second}};
                out["executor"].push_back(jact);
            }
        } else if (auto x = dynamic_cast<const ast::MatcherDefCond *> (node)) {
            out["type"] = "condition";
            out["conds"] = json::array();
            for (auto &cond: x->conds) {
                out["conds"].push_back(visit(cond.get(), ctx));
            }
        } else if (auto x = dynamic_cast<const ast::MatcherDefTable *>(node )) {
            out["type"] = "table";
            out["name"] = x->table_name;
        } else {
            throw std::runtime_error("Unexpected node type");
        }
        return out;
    }
};
