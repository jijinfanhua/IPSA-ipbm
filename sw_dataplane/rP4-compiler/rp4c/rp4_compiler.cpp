#include "rp4_compiler.h"
#include "ipsa_output.h"
#include "ipsa_builder.h"

IpsaBuilder builder;

// test the compilation (rp4 -> json) module
int main(int argc, char* argv[]) {
    const char* input = argv[1];
    const char* output = argv[2];
    Rp4Compiler compiler;
    if (compiler.parse(input) == 0) {
        builder.load(compiler.ast.get());
        builder.allocateParsers();
        builder.allocateMemory();
        builder.allocateProcessors();
        freopen(output, "w", stdout);
        IpsaOutput out(std::cout);
        // for (auto& gateway : builder.gateway_manager.gateways) {
        //     out.emit(gateway.toIpsaValue());
        // }
        // for (auto& [name, table]: builder.table_manager.tables) {
        //     out.emit(table.toIpsaValue());
        // }
        // for (auto& [name, action] : builder.action_manager.actions) {
        //     out.emit(action.toIpsaValue());
        // }
        // for (auto& level : builder.level_manager.levels) {
        //     out.emit(level.toIpsaValue());
        // }
        // compiler.print(output);
        out.emit(builder.ipsa.toIpsaValue());
    } else {
        std::cout << "parse error" << std::endl;
    }
    return 0;
}