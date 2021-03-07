#include <fstream>
#include <iostream>
#include <string>

#include "codegen.hpp"
#include "debug.hpp"
#include "grammar.hpp"

std::string read_file(const char* filename) {
    std::ifstream ifs(filename);
    std::string ret;

    ifs.seekg(0, std::ios::end);
    ret.reserve(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    ret.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    return ret;
}

int main() {
    // pc::StringInput in(read_file("./examples/middle-expression.m"));
    // pc::StringInput in(read_file("./examples/simple-a-plus-b.m"));
    // pc::StringInput in(read_file("./examples/simple-variable.m"));
    pc::StringInput in(read_file("./examples/various-space-and-tabs.m"));
    auto ret = pc::parse(program, in);
    print_tree_debug(*ret);
}
