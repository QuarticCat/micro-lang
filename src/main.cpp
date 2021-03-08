#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "codegen.hpp"
#include "debug.hpp"
#include "grammar.hpp"

/// Remove comments from istream and return processed string.
std::string preprocess(std::istream& is) {
    std::string ret;
    std::string line;
    while (std::getline(is, line)) {
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '-' && line[i + 1] == '-') {
                line[i] = '\0';
                break;
            }
        }
        (ret += line.data()) += '\n';
    }
    return ret;
}

int main() {
    // std::ifstream ifs("./examples/middle-expression.m");
    // std::ifstream ifs("./examples/simple-a-plus-b.m");
    // std::ifstream ifs("./examples/simple-variable.m");
    std::ifstream ifs("./examples/various-space-and-tabs.m");
    pc::StringInput in(preprocess(ifs));
    auto ret = pc::parse(program, in);
    if (!ret) throw std::logic_error("Parse Error");
    print_tree_debug(*ret);
    auto code = codegen(*ret);
    std::cout << code;
}
