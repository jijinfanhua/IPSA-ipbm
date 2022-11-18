#include "ipsa_action_manager.h"
#include "ipsa_allocate.h"
#include "ipsa_builder.h"
#include "ipsa_expression.h"
#include "ipsa_gateway.h"
#include "ipsa_gateway_manager.h"
#include "ipsa_header_manager.h"
#include "ipsa_level_manager.h"
#include "ipsa_module.h"
#include "ipsa_output.h"
#include "ipsa_processor_manager.h"
#include "ipsa_stage_manager.h"
#include "ipsa_table.h"
#include "ipsa_table_manager.h"
#include "ipsa_value.h"
#include "rp4_table.h"
#include "rp4_treenode.h"

std::vector<IpsaHeaderField *> IpsaHeaderField::ipsaHeaderFields;

std::string to_string(IpsaOperandType op) {
    static const std::map<IpsaOperandType, std::string> m = {
            {OPT_CONST, "CONST"},
            {OPT_EXPR, "EXPR"},
            {OPT_FIELD, "FIELD"},
            {OPT_PARAM, "PARAM"}};
    return m.at(op);
}

std::string to_string(IpsaMemoryType mt) {
    static const std::map<IpsaMemoryType, std::string> m = {
            {MEM_TCAM, "TCAM"},
            {MEM_SRAM, "SRAM"}};
    return m.at(mt);
}

std::shared_ptr<IpsaValue> makeValue(int _value) {
    return std::make_shared<IpsaInteger>(_value);
}

std::shared_ptr<IpsaValue> makeValue(std::string _value) {
    return std::make_shared<IpsaString>(_value);
}

std::shared_ptr<IpsaValue> makeValue(std::vector<std::shared_ptr<IpsaValue>> _value) {
    return std::make_shared<IpsaList>(_value);
}

std::shared_ptr<IpsaValue> makeValue(std::map<std::string, std::shared_ptr<IpsaValue>> _value) {
    return std::make_shared<IpsaDict>(_value);
}

std::string to_string(Rp4Operator op) {
    static const std::map<Rp4Operator, std::string> m = {
            {OP_EQ, "=="},
            {OP_PLUS, "+"},
            {OP_MUL, "*"},
            {OP_NE, "!="},
            {OP_MINUS, "-"},
            {OP_DIV, "/"},
            {OP_LT, "<"},
            {OP_AND, "&"},
            {OP_NOTL, "!"},
            {OP_GT, ">"},
            {OP_OR, "|"},
            {OP_LE, "<="},
            {OP_XOR, "^"},
            {OP_GE, ">="},
            {OP_NOT, "~"}};
    return m.at(op);
}

std::string to_string(Rp4MatchType mt) {
    static const std::map<Rp4MatchType, std::string> m = {
            {MT_EXACT, "EXACT"},
            {MT_TERNARY, "TERNARY"},
            {MT_LPM, "LPM"}};
    return m.at(mt);
}

const IpsaAction *IpsaActionManager::lookupConst(std::string name) const {
    if (auto x = actions.find(name); x != std::end(actions)) {
        return &(x->second);
    } else {
        return nullptr;
    }
}

IpsaAction *IpsaActionManager::lookup(std::string name) {
    if (auto x = actions.find(name); x != std::end(actions)) {
        return &(x->second);
    } else {
        return nullptr;
    }
}

IpsaAction *IpsaActionManager::lookup(int action_id) {
    for (auto &[name, action]: actions) {
        if (action.id == action_id) {
            return &action;
        }
    }
    return nullptr;
}

std::shared_ptr<IpsaOperand> parse_expr(
        std::shared_ptr<Rp4Operation> v,
        const IpsaHeaderManager *header_manager,
        const Rp4ActionDef *action_def) {
    if (v->isLiteral()) {
        return std::make_shared<IpsaConstant>(std::static_pointer_cast<Rp4Literal>(v)->value);
    } else if (v->isLValue()) {
        return std::make_shared<IpsaField>(
                *header_manager->lookup(std::static_pointer_cast<Rp4LValue>(v)));
    } else if (v->isParameter()) {
        auto &name = std::static_pointer_cast<Rp4Parameter>(v)->name;
        for (int i = 0; i < action_def->parameters.size(); i++) {
            if (action_def->parameters[i].name == name) {
                return std::make_shared<IpsaParameter>(i);
            }
        }
    } else if (v->isBinary()) {
        auto x = std::static_pointer_cast<Rp4Binary>(v);
        return std::make_shared<IpsaExpression>(
                x->op,
                parse_expr(x->left, header_manager, action_def),
                parse_expr(x->right, header_manager, action_def));
    }
    return nullptr;
}

void IpsaActionManager::addAction(const Rp4ActionDef &_action_def, const IpsaHeaderManager *header_manager) {
    if (actions.find(_action_def.name) == std::end(actions)) {
        IpsaAction action(global_action_id++, _action_def);
        action.parameter_num = action.action_def.parameters.size();
        for (auto &param: action.action_def.parameters) {
            auto type = param.type;
            int length = 0;
            if (type->isBitType()) {
                length = std::static_pointer_cast<Rp4BitType>(type)->length;
            }
            action.action_parameters_lengths.push_back(length);
        }
        for (auto &expression: action.action_def.expressions) {
            action.primitives.push_back(IpsaAssign(
                    header_manager->lookup(expression.lvalue),
                    parse_expr(expression.rvalue, header_manager, &action.action_def)));
        }
        actions.insert({{action.action_def.name, std::move(action)}});
    }
}

void IpsaActionManager::reloadPrimitives(const IpsaHeaderManager *header_manager) {
    for (auto &[name, action]: actions) {
        action.primitives.clear();
        for (auto &expression: action.action_def.expressions) {
            action.primitives.push_back(IpsaAssign(
                    header_manager->lookup(expression.lvalue),
                    parse_expr(expression.rvalue, header_manager, &action.action_def)));
        }
    }
}

IpsaExpression expr_to_expr(std::shared_ptr<Rp4Operation> v, const IpsaHeaderManager *header_manager) {
    auto x = parse_expr(v, header_manager, nullptr);
    if (x->isConstant() || x->isField()) {
        return IpsaExpression(
                OP_EQ, std::make_shared<IpsaConstant>(1), x);
    } else {
        return IpsaExpression(
                *std::static_pointer_cast<IpsaExpression>(x).get());
    }
}

void IpsaGatewayManager::load(
        IpsaHeaderManager *header_manager,
        IpsaStageManager *stage_manager,// after initialization
        IpsaTableManager *table_manager) {
    global_gateway_id = 0;
    auto make_gateway_entry = [&](const Rp4SwitchEntry &entry) -> std::shared_ptr<IpsaGatewayEntry> {
        if (entry.value->isTableStmt()) {
            auto name = std::static_pointer_cast<Rp4SwitchTableStmt>(entry.value)->name;
            return std::make_shared<IpsaGatewayTableEntry>(
                    table_manager->lookup(name)->TableId());
        } else {
            auto x = std::static_pointer_cast<Rp4SwitchStageStmt>(entry.value);
            if (x->to_none) {
                return std::make_shared<IpsaGatewayStageEntry>(-1);
            } else {
                return std::make_shared<IpsaGatewayStageEntry>(
                        stage_manager->lookup(x->name)->stage_id);
            }
        }
    };
    for (auto &[name, stage]: stage_manager->logical_stages) {
        auto &matcher = stage.def.matcher;
        IpsaGateway gateway;
        for (auto op: matcher.ops) {
            gateway.expressions.push_back(expr_to_expr(op, header_manager));
        }
        gateway.next_table.default_entry = nullptr;
        for (auto &entry: matcher.switch_entries) {
            if (entry.key->isDefault()) {
                gateway.next_table.default_entry = make_gateway_entry(entry);
            } else {
                gateway.next_table.entries.push_back(
                        IpsaNextTableEntry(
                                (entry.key->get_key() << (ipsa_configuration::BIT_SIZE - gateway.expressions.size())),
                                make_gateway_entry(entry)));
            }
        }
        if (gateway.next_table.default_entry.get() == nullptr) {
            gateway.next_table.default_entry = gateway.next_table.entries[0].value;
        }
        gateways.push_back(std::move(gateway));

        stage.gateway_id = global_gateway_id++;
    }
}

// enum IpsaGatewayEntryType {
//     GTW_ET_TABLE, GTW_ET_STAGE
// };

std::string to_string(IpsaGatewayEntryType op) {
    static const std::map<IpsaGatewayEntryType, std::string> m = {
            {GTW_ET_TABLE, "TABLE"},
            {GTW_ET_STAGE, "STAGE"}};
    return m.at(op);
}

// enum IpsaFieldType {
//     FT_FIELD, FT_VALID, FT_HIT, FT_MISS
// };

std::string to_string(IpsaFieldType op) {
    static const std::map<IpsaFieldType, std::string> m = {
            {FT_FIELD, "FIELD"},
            {FT_VALID, "VALID"},
            {FT_HIT, "HIT"},
            {FT_MISS, "MISS"}};
    return m.at(op);
}

int IpsaHeader::getLength() const {
    int width = 0;
    for (auto &[name, field]: fields) {
        width += field->FieldLength();
    }
    return (width + 7) / 8;
}

const IpsaHeaderField *IpsaHeader::lookup(std::string field_name) const {
    if (auto x = fields.find(field_name); x != std::end(fields)) {
        return x->second;
    } else {
        std::cout << "error invalid field: " << field_name << std::endl;
        return nullptr;
    }
}

IpsaHeader::IpsaHeader(const Rp4HeaderDef *header_def, int id) : header_id(id), is_header(true), is_standard_metadata(false) {
    int total_length = 0;
    offset = 0;
    for (auto &field: header_def->fields) {
        if (auto x = field.type; x->isBitType()) {
            auto y = dynamic_cast<const Rp4BitType *>(x.get());
            fields.insert({{field.name, IpsaHeaderField::CreateHeaderField(
                                                FT_FIELD, id, offset, y->length)}});
            offset += y->length;
            total_length += y->length;
        } else {
            std::cout << "error header: " << header_def->name << std::endl;
        }
    }
    fields.insert({{"isValid", IpsaHeaderField::CreateHeaderField(FT_VALID, id, 0, total_length)}});
}

IpsaHeader::IpsaHeader(const Rp4StructDef *struct_def, int id) : header_id(id), is_header(false) {
    if (struct_def->name == "standard_metadata") {
        is_standard_metadata = true;
    }
    fields.insert({{"isValid", IpsaHeaderField::CreateHeaderField(FT_VALID, id, 0, 0)}});
    offset = 0;
    for (auto &field: struct_def->fields) {
        if (auto x = field.type; x->isBitType()) {
            auto y = dynamic_cast<const Rp4BitType *>(x.get());
            fields.insert({{field.name, IpsaHeaderField::CreateHeaderField(
                                                FT_FIELD, id, offset, y->length)}});
            offset += y->length;
        } else {
            std::cout << "error metadata: " << struct_def->struct_name << std::endl;
        }
    }
}

const IpsaHeader *IpsaHeaderManager::get_header(int header_id) const {
    for (auto &[name, header]: headers) {
        if (header.header_id == header_id) {
            return &header;
        }
    }
    return nullptr;
}

const IpsaHeader *IpsaHeaderManager::get_header(const std::string &name) const {
    if (auto x = headers.find(name); x != std::end(headers)) {
        return &(x->second);
    } else {
        std::cout << "error wrong header name: " << name << std::endl;
        return nullptr;
    }
}

const IpsaHeader *IpsaHeaderManager::get_header(const Rp4Member *member) const {
    if (member->instance_name == this->header_name) {
        return get_header(member->member_name);
    } else {
        std::cout << "error wrong headers name: " << member->instance_name << std::endl;
        return nullptr;
    }
}

const IpsaHeaderField *IpsaHeaderManager::lookup(std::string header_name, std::string field_name) const {
    if (auto x = headers.find(header_name); x != std::end(headers)) {
        return x->second.lookup(field_name);
    } else {
        std::cout << "error header name: " << header_name << std::endl;
        return nullptr;
    }
}

const IpsaHeaderField *IpsaHeaderManager::lookup(std::shared_ptr<Rp4LValue> lvalue) const {
    std::string header_name, field_name;
    if (lvalue->isMeta()) {
        auto member = dynamic_cast<const Rp4MetaLValue *>(lvalue.get())->field;
        header_name = member.instance_name;
        field_name = member.member_name;
    } else if (lvalue->isHeader()) {
        auto header = dynamic_cast<const Rp4HeaderLValue *>(lvalue.get())->field;
        if (header.instance_name == this->header_name) {
            header_name = header.member_name;
            field_name = header.field_name;
        } else {
            std::cout << "error wrong headers name: " << header.instance_name << std::endl;
            return nullptr;
        }
    } else {
        return nullptr;
    }
    return lookup(std::move(header_name), std::move(field_name));
}

void IpsaHeaderManager::addHeader(const Rp4HeaderDef *header_def, std::string name) {
    if (headers.find(name) == headers.end()) {
        headers.insert({{name, IpsaHeader(header_def, global_header_id++)}});
    }
}

void IpsaHeaderManager::addMetadata(const Rp4StructDef *struct_def) {
    if (headers.find(struct_def->name) == headers.end()) {
        int header_id = global_header_id;
        if (struct_def->name == "standard_metadata") {
            header_id = 31;// fixed
        } else {
            global_header_id++;
        }
        headers.insert({
                {struct_def->name, IpsaHeader(struct_def, header_id)},
        });
    }
}

void IpsaHeaderManager::load(const std::vector<Rp4StructDef> &struct_defs, const std::vector<Rp4HeaderDef> &header_defs) {
    global_header_id = 0;
    headers.clear();
    metadata.entries.clear();
    for (auto &struct_def: struct_defs) {
        if (struct_def.is_header) {
            header_name = struct_def.name;
            for (auto &field_def: struct_def.fields) {
                if (auto x = field_def.type; x->isNamedType()) {
                    auto y = dynamic_cast<const Rp4NamedType *>(x.get());
                    if (auto header_def = std::find_if(std::begin(header_defs), std::end(header_defs), [&](auto &h) { return h.name == y->name; });
                        header_def != std::end(header_defs)) {
                        addHeader(header_def.base(), field_def.name);
                    } else {
                        std::cout << "error no header type: " << y->name << std::endl;
                    }
                } else {
                    std::cout << "error bit type in headers: " << field_def.name << std::endl;
                }
            }
        } else {
            addMetadata(&struct_def);
        }
    }
    // generate metadata
    metadata.entries.push_back(IpsaMetadataEntry(31, 0, 20));
    int offset = 20;
    for (auto &[name, header]: headers) {
        if (!header.is_header && !header.is_standard_metadata) {
            int length = std::max(8, header.getLength() * 2);
            metadata.entries.push_back(IpsaMetadataEntry(
                    header.header_id, offset, length));
            offset += length;
        }
    }
}

void IpsaLevelManager::load(const Rp4ParserDef &parser_def, const IpsaHeaderManager *header_manager) {
    levels.clear();
    states.clear();
    global_level_id = 0;

    auto &state_defs = parser_def.states;
    auto get_state = [&](std::string state_name) -> const Rp4StateDef * {
        if (auto state_def = std::find_if(std::begin(state_defs), std::end(state_defs), [&](auto &state) { return state.name == state_name; });
            state_def != std::end(state_defs)) {
            return state_def.base();
        } else {
            return nullptr;
        }
    };
    // auto get_miss_act = [&](const Rp4StateDef* state_def) {
    //     if (state_def != nullptr) {
    //         auto trans = state_def->transition;
    //         Rp4TransitionEntry* default_entry = nullptr;
    //         if (trans->isDirect()) {
    //             default_entry = &(std::static_pointer_cast<Rp4DirectTransition>(trans)->entry);
    //         } else if (trans->isSelect()) {
    //             auto& entries = std::static_pointer_cast<Rp4SelectTransition>(trans)->entries;
    //             if (auto de = std::find_if(std::begin(entries), std::end(entries), [](auto& entry) {
    //                 return entry.key->isDefault();
    //             }); de != std::end(entries)) {
    //                 if (de->is_accept) {
    //                     return 0;
    //                 }
    //             }
    //         }
    //     }
    //     return 1;
    // };
    auto get_select = [&](const Rp4StateDef *state_def) -> std::vector<const IpsaHeaderField *> {
        if (state_def == nullptr) {
            return {};
        }
        auto trans = state_def->transition;
        if (trans->isDirect()) {
            return {};
        }
        auto &targets = std::static_pointer_cast<Rp4SelectTransition>(trans)->targets;
        std::vector<const IpsaHeaderField *> fields;
        for (auto &target: targets) {
            fields.push_back(header_manager->lookup(
                    target.member_name, target.field_name));
        }
        return std::move(fields);
    };
    auto build_entries = [&](IpsaLevel &level, int state, const std::vector<Rp4TransitionEntry> &entries) {
        for (auto &entry: entries) {
            if (entry.accept_drop == 2) {
                auto next_state = get_state(entry.next);
                auto m = header_manager->get_header(&(next_state->extract.target));
                level.add(IpsaLevelEntry(
                        IpsaParserTcamEntry(state, entry.key.get()),
                        IpsaParserSramEntry(
                                m->header_id, m->offset,
                                states[next_state->name],
                                get_select(next_state), 2)));
            } else {
                level.add(IpsaLevelEntry(
                        IpsaParserTcamEntry(state, entry.key.get()),
                        IpsaParserSramEntry(0, 0, 0, {}, entry.accept_drop)));
            }
        }
    };
    if (state_defs.size() > 0) {
        auto first_state = &(state_defs[0]);
        auto &first_header = state_defs[0].extract.target;
        auto m = header_manager->get_header(&first_header);
        states.insert({{"", global_level_id++}});
        levels.push_back(IpsaLevel());
        for (auto &state_def: state_defs) {
            states.insert({{state_def.name, global_level_id++}});
            levels.push_back(IpsaLevel());
        }
        levels[0].add(IpsaLevelEntry(
                IpsaParserTcamEntry(0, 0, 0),
                IpsaParserSramEntry(
                        m->header_id, m->offset, states[first_state->name],
                        get_select(first_state), 2)));
        for (auto &state_def: state_defs) {
            if (auto x = states.find(state_def.name); x != std::end(states)) {
                auto &level = levels[x->second];
                auto trans = state_def.transition;
                if (trans->isDirect()) {
                    build_entries(level, x->second, {std::static_pointer_cast<Rp4DirectTransition>(trans)->entry});
                } else if (trans->isSelect()) {
                    build_entries(level, x->second, std::static_pointer_cast<Rp4SelectTransition>(trans)->entries);
                }
            }
        }
    }
}

std::shared_ptr<IpsaValue> makeValue(const std::vector<int> &v) {
    std::vector<std::shared_ptr<IpsaValue>> temp;
    for (auto x: v) {
        temp.push_back(makeValue(x));
    }
    return makeValue(temp);
}

std::ostream &IpsaOutput::emit(std::shared_ptr<IpsaValue> value) {
    if (value.get() == nullptr) {
        out << "null";
    } else if (value->isInteger()) {
        out << std::static_pointer_cast<IpsaInteger>(value)->getValue();
    } else if (value->isString()) {
        out << "\"" << std::static_pointer_cast<IpsaString>(value)->getValue() << "\"";
    } else if (value->isList()) {
        auto &l = std::static_pointer_cast<IpsaList>(value)->getValue();
        if (l.size() == 0) {
            out << "[]";
        } else {
            out << "[" << std::endl;
            tab += 2;
            std::string offset(tab, ' ');
            for (auto c = std::begin(l); c != std::end(l);) {
                out << offset;
                emit(*c);
                if (++c != std::end(l)) {
                    out << ",";
                }
                out << std::endl;
            }
            tab -= 2;
            out << std::string(tab, ' ') << "]";
        }
    } else if (value->isDict()) {
        auto &d = std::static_pointer_cast<IpsaDict>(value)->getValue();
        if (d.size() == 0) {
            out << "{}";
        } else {
            out << "{" << std::endl;
            tab += 2;
            std::string offset(tab, ' ');
            for (auto c = std::begin(d); c != std::end(d);) {
                out << offset;
                out << "\"" << c->first << "\" : ";
                emit(c->second);
                if (++c != std::end(d)) {
                    out << ",";
                }
                out << std::endl;
            }
            tab -= 2;
            out << std::string(tab, ' ') << "}";
        }
    }
    return out;
}

void IpsaProcessorManager::initializeStages() {
    // action to proc table
    for (auto &[name, stage]: stage_manager->logical_stages) {
        stage.action_proc.clear();
        auto &exec_entries = stage.def.executor.entries;
        for (auto &entry: exec_entries) {
            auto action = action_manager->lookupConst(entry.action_name);
            int action_id = action == nullptr ? 0 : action->id;
            auto processor = stage_manager->lookup(entry.stage_name);
            int processor_id = processor == nullptr ? -1 : processor->stage_id;
            stage.action_proc.push_back({action_id, processor_id});
        }
    }
    for (auto &[name, stage]: stage_manager->logical_stages) {
        stage.table_id.clear();
        for (auto &switch_entry: stage.def.matcher.switch_entries) {
            if (switch_entry.value->isTableStmt()) {
                auto &name = std::static_pointer_cast<Rp4SwitchTableStmt>(switch_entry.value)->name;
                stage.table_id.push_back(table_manager->lookup(name)->table_id);
            }
        }
    }
}

void IpsaProcessorManager::reorderStages(std::map<int, int> proc_proc) {
    for (auto &[name, stage]: stage_manager->logical_stages) {
        stage.stage_id = proc_proc[stage.stage_id];
        for (int i = 0; i < stage.action_proc.size(); i++) {
            stage.action_proc[i] = {
                    stage.action_proc[i].first,
                    proc_proc[stage.action_proc[i].second]};
        }
    }
}

void IpsaProcessorManager::setupStages(IpsaGatewayManager *gateway_manager) {
    // delete tables.action_to_proc
    for (auto &[name, table]: table_manager->tables) {
        table.action_to_proc.clear();
    }

    // set id in tables and the corresponding gateway entries (with action2proc map)
    for (auto &[name, stage]: stage_manager->logical_stages) {
        int matcher_id = 0;
        for (int table_id: stage.table_id) {
            auto &gateway = gateway_manager->gateways[stage.gateway_id];
            table_manager->setMatcherId(table_id, stage.stage_id, matcher_id, stage.action_proc);
            gateway.next_table.setMatcherId(table_id, matcher_id++);
        }
    }
}

void IpsaStageManager::load(const std::unique_ptr<Rp4Ast>& ast) {
    for (auto &stage_def: ast->ingress_def.stage_defs) {
        addStage(&stage_def, true);
    }
    for (auto &stage_def: ast->egress_def.stage_defs) {
        addStage(&stage_def, false);
    }
}

void IpsaStageManager::addStage(const Rp4StageDef *stage_def, bool in_ingress) {
    if (logical_stages.find(stage_def->name) == std::end(logical_stages)) {
        logical_stages.insert({{stage_def->name, IpsaStage(stage_def, global_stage_id++, in_ingress)}});
    }
}

IpsaStage *IpsaStageManager::lookup(std::string stage_name) {
    if (auto x = logical_stages.find(stage_name); x != std::end(logical_stages)) {
        return &x->second;
    } else {
        return nullptr;
    }
}

IpsaStage *IpsaStageManager::lookup(int stage_id) {
    for (auto &[name, stage]: logical_stages) {
        if (stage.stage_id == stage_id) {
            return &stage;
        }
    }
    return nullptr;
}

// also set action_to_proc list
void IpsaTableManager::setMatcherId(int table_id, int proc_id, int matcher_id, const std::vector<std::pair<int, int>> &action_proc) {
    for (auto &[name, table]: tables) {
        if (table.TableId() == table_id) {
            table.id = matcher_id;
            for (auto [action, proc]: action_proc) {
                table.action_to_proc.push_back(IpsaActionProcPair(action, proc));
            }
            break;
        }
    }
}

// void IpsaTableManager::reorderStages(std::map<int, int> proc_proc) {
//     for (auto& [name, table] : tables) {
//         table.proc_id = proc_proc[table.proc_id];
//         for (auto& [action, proc] : table.action_to_proc) {
//             proc = proc_proc[proc];
//         }
//     }
// }

IpsaTable *IpsaTableManager::lookup(int table_id) {
    for (auto &[name, table]: tables) {
        if (table.TableId() == table_id) {
            return &table;
        }
    }
    return nullptr;
}

IpsaTable *IpsaTableManager::lookup(std::string name) {
    if (auto x = tables.find(name); x != std::end(tables)) {
        return &(x->second);
    } else {
        return nullptr;
    }
}

// leave id, action to proc and memory config undefined
void IpsaTableManager::load(const std::unique_ptr<Rp4Ast> &ast, const IpsaHeaderManager *header_manager, const IpsaActionManager *action_manager) {
    for (const auto &table_def: ast->tables_def.tables) {
        addTable(table_def, header_manager, action_manager);
    }
}

void IpsaTableManager::addTable(const Rp4TableDef &table_def, const IpsaHeaderManager *header_manager, const IpsaActionManager *action_manager) {
    IpsaTable table = IpsaTable(global_table_id++, table_def);
    auto &key_def_entries = table_def.key_def.entries;
    int default_action_id = 0;
    for (auto x: table_def.optional_stmts) {
        if (x->isDefault()) {
            auto &y = std::static_pointer_cast<Rp4DefaultActionStmt>(x)->action_name;
            auto default_action = action_manager->lookupConst(y);
            if (default_action != nullptr) {
                default_action_id = default_action->id;
            }
        }
    }
    table.miss_action_id = default_action_id;
    if (key_def_entries.size() == 0) {
        table.no_table = 1;// no entries, no table
        table.key_memory.depth = table.key_memory.width = table.value_memory.depth = table.value_memory.width = 0;
    } else {
        table.no_table = 0;
        if (key_def_entries.size() == 1) {
            auto &entry = key_def_entries[0];
            table.match_type = entry.match_type;
        } else {
            if (std::all_of(std::begin(key_def_entries), std::end(key_def_entries), [](auto &entry) { return entry.match_type == MT_EXACT; })) {
                table.match_type = MT_EXACT;
            } else {
                table.match_type = MT_TERNARY;
            }
        }
        if (table.match_type == MT_EXACT) {
            table.key_memory.type = MEM_SRAM;
        } else {
            table.key_memory.type = MEM_TCAM;
        }
        int table_key_width = 0;
        for (auto &entry: key_def_entries) {
            auto x = header_manager->lookup(entry.lvalue);
            table.key_width_vec.push_back(x->FieldLength());
            table_key_width += x->FieldLength();
            table.field_infos.push_back(x->toIpsaValue());
        }
        int mem_width = table.key_memory.type == MEM_SRAM ? ipsa_configuration::SRAM_WIDTH : ipsa_configuration::TCAM_WIDTH;
        int mem_depth = table.key_memory.type == MEM_SRAM ? ipsa_configuration::SRAM_DEPTH : ipsa_configuration::TCAM_DEPTH;
        table.key_memory.width = (table_key_width + (mem_width - 1)) / mem_width;
        int depth = 1;
        for (auto x: table_def.optional_stmts) {
            if (x->isSize()) {
                depth = std::static_pointer_cast<Rp4TableSizeStmt>(x)->size;
                depth = (depth + (mem_depth - 1)) / mem_depth;
                break;
            }
        }
        table.key_memory.depth = table.value_memory.depth = depth;
        table.value_memory.type = MEM_SRAM;
        mem_width = ipsa_configuration::SRAM_WIDTH;
        int width = 1;
        for (auto x: table_def.optional_stmts) {
            if (x->isWidth()) {
                width = std::static_pointer_cast<Rp4TableValueWidthStmt>(x)->width;
                width = (width + (mem_width - 1)) / mem_width;
                break;
            }
        }
        table.value_memory.width = width;
    }
    tables.insert({{table_def.name, table}});
}

void IpsaTableManager::reloadHeader(const IpsaHeaderManager *header_manager) {
    for (auto &[name, table]: tables)
        if (!table.no_table) {
            auto &key_def_entries = table.table_def.key_def.entries;
            int table_key_width = 0;
            table.key_width_vec.clear();
            table.field_infos.clear();
            for (auto &entry: key_def_entries) {
                auto x = header_manager->lookup(entry.lvalue);
                table.key_width_vec.push_back(x->FieldLength());
                table_key_width += x->FieldLength();
                table.field_infos.push_back(x->toIpsaValue());
            }
        }
}