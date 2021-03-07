#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "asm.hpp"
#include "grammar.hpp"

namespace detail {

template<typename U, typename... Fs>
auto match(U&& u, Fs... arms) {
    struct overloaded: public Fs... {
        using Fs::operator()...;
    };
    return std::visit(overloaded{arms...}, std::forward<U>(u));
}

struct Val {
    int offset;
};
struct Imm {
    int num;
};
using Operand = std::variant<Val, Imm>;

struct Generator {
    int stack_size = 32;
    std::unordered_map<std::string_view, int> offset_map{};
    std::vector<Chars> output{};

    std::string operator()(pc::Token& token) {
        this->gen(token);
        std::string ret = prelude;

        auto chars = main_prologue(this->stack_size);
        ret += chars.release();

        for (auto& p: output) ret += p.get();

        chars = main_epilogue(this->stack_size);
        ret += chars.release();

        return ret;
    }

    void gen(pc::Token& token) {
        switch (token.tag()) {
        case program.tag: {
            for (auto& child: token.children) this->gen(child);
            break;
        }
        case read_stmt.tag: {
            for (auto& child: token.children) {
                auto val = std::get<Val>(this->eval(child));
                this->output.push_back(read(val.offset));
            }
            break;
        }
        case write_stmt.tag: {
            for (auto& child: token.children) {
                this->output.push_back(match(
                    this->eval(child),
                    [](Val val) { return write(val.offset); },
                    [](Imm imm) { return writei(imm.num); }));
            }
            break;
        }
        case assign_stmt.tag: {
            auto lhs = std::get<Val>(this->eval(token.children[0]));
            this->output.push_back(match(
                this->eval(token.children[1]),
                [lhs](Val val) { return move(lhs.offset, val.offset); },
                [lhs](Imm imm) { return movei(lhs.offset, imm.num); }));
            break;
        }
        default: {
            throw std::domain_error("Unexpected tag!");
        }
        }
    }

    Operand eval(pc::Token& token) {
        switch (token.tag()) {
        case expr.tag: {
            auto ret = eval(token.children[0]);
            if (token.children.size() == 1) return ret;
            auto iter = token.children.begin() + 1;
            while (iter != token.children.end()) {
                char op = *iter++->begin();
                auto next = eval(*iter++);
                auto dst = this->stack_size;
                this->stack_size += 4;
                if (std::holds_alternative<Val>(ret) && std::holds_alternative<Val>(next)) {
                    auto lhs = std::get<Val>(ret).offset, rhs = std::get<Val>(next).offset;
                    this->output.push_back(op == '+' ? add(lhs, rhs, dst) : sub(lhs, rhs, dst));
                    ret = Val{dst};
                } else if (std::holds_alternative<Val>(ret) && std::holds_alternative<Imm>(next)) {
                    auto lhs = std::get<Val>(ret).offset, rhs = std::get<Imm>(next).num;
                    this->output.push_back(op == '+' ? addi(lhs, rhs, dst) : addi(lhs, -rhs, dst));
                    ret = Val{dst};
                } else if (std::holds_alternative<Imm>(ret) && std::holds_alternative<Val>(next)) {
                    auto lhs = std::get<Imm>(ret).num, rhs = std::get<Val>(next).offset;
                    this->output.push_back(op == '+' ? addi(rhs, lhs, dst) : isub(lhs, rhs, dst));
                    ret = Val{dst};
                } else {
                    auto lhs = std::get<Imm>(ret).num, rhs = std::get<Imm>(next).num;
                    ret = Imm{op == '+' ? lhs + rhs : lhs - rhs};
                }
            }
            return ret;
        }
        case ident.tag: {
            if (!this->offset_map.contains(token.view())) {
                this->offset_map[token.view()] = this->stack_size;
                this->stack_size += 4;
            }
            return Val{this->offset_map[token.view()]};
        }
        case integer.tag: {
            auto iter = token.begin();
            bool is_positive = *iter == '-';
            if (*iter == '-' || *iter == '+') ++iter;
            int ret = 0;
            for (; iter != token.end(); ++iter) ret = ret * 10 + (*iter - '0');
            return Imm{is_positive ? ret : -ret};
        }
        // case add_op.tag:
        //     break;
        default:
            throw std::domain_error("Unexpected tag!");
        }
    }
};

}  // namespace detail

std::string codegen(pc::Token& token) {
    return detail::Generator{}(token);
}
