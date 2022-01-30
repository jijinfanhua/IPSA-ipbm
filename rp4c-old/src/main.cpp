#include "parser/parser.h"
#include "backend/json_emitter.h"
#include <iostream>
#include <fstream>

void print_usage(const char *name) {
    std::cout << "Usage: " << name << " PROG_FILE\n";
}

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *filename = argv[1];
    Parser parser;
    if (parser.parse(filename) == 0) {
        parser.print();
    }
    auto j = JsonEmitter::emit(parser.tree.get());
    std::ofstream ofs("out.json");
    ofs << j.dump(4);
    return 0;
}
