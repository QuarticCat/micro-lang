#pragma once

#include <cstddef>
#include <cstdio>
#include <iostream>

#include "grammar.hpp"

#define PRINT(rule)                                                         \
    case rule.tag:                                                          \
        std::cout << #rule "\n";                                            \
        for (auto& child: token.children) print_tree(child, indent + Step); \
        break

template<size_t Step = 2>
void print_tree(pc::Token& token, size_t indent = 0) {
    for (size_t i = 0; i < indent; ++i) putchar(' ');
    switch (token.tag()) {
        PRINT(program);
        PRINT(read_stmt);
        PRINT(write_stmt);
        PRINT(assign_stmt);
        PRINT(expr);
        PRINT(ident);
        PRINT(integer);
    default:
        std::cout << "ERROR!\n";
        break;
    }
}

#undef PRINT
