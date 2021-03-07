#pragma once

#include <cstddef>
#include <cstdio>
#include <iostream>

#include "grammar.hpp"

namespace detail {

#define CASE(rule)                                                              \
    case rule.tag:                                                              \
        std::cout << #rule;                                                     \
        if (token.children.empty()) {                                           \
            std::cout << " : \"" << token.view() << "\"\n";                     \
        } else if (token.children.size() == 1) {                                \
            std::cout << " > ";                                                 \
            print_tree(token.children[0], indent + Step, false);                \
        } else {                                                                \
            std::cout << '\n';                                                  \
            for (auto& child: token.children) print_tree(child, indent + Step); \
        }                                                                       \
        break

template<size_t Step = 2>
void print_tree(pc::Token& token, size_t indent = 0, bool newline = true) {
    if (newline) {
        for (size_t i = 0; i < indent; ++i) putchar(' ');
        putchar('-');
        putchar(' ');
    }
    switch (token.tag()) {
        CASE(program);
        CASE(read_stmt);
        CASE(write_stmt);
        CASE(assign_stmt);
        CASE(expr);
        CASE(ident);
        CASE(integer);
        CASE(add_op);
    default:
        std::cout << "ERROR!\n";
        break;
    }
}

#undef CASE

}  // namespace detail

template<size_t Step = 2>
void print_tree_debug([[maybe_unused]] pc::Token& token) {
#ifndef NDEBUG
    detail::print_tree<Step>(token);
#endif
}
