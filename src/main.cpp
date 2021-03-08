#include <fstream>
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

int main(int argc, char* argv[]) {
    if (argc != 3) throw std::invalid_argument("Wrong argument number!");

    std::ifstream ifs(argv[1]);
    if (ifs.fail()) throw std::invalid_argument("Unable to open input file!");

    pc::StringInput in(preprocess(ifs));
    auto ret = pc::parse(program, in);
    if (!ret) throw std::logic_error("Parse error!");
    print_tree_debug(*ret);
    auto code = codegen(*ret);

    std::ofstream ofs(argv[2]);
    if (ofs.fail()) throw std::invalid_argument("Unable to open output file!");
    ofs << code;
}
