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

/// A handy function for doing fake pattern-matching.
template<typename U, typename... Fs>
auto match(U&& u, Fs... arms) {
    struct overloaded: public Fs... {
        using Fs::operator()...;
    };
    return std::visit(overloaded{arms...}, std::forward<U>(u));
}

/// Represent a memory position.
struct Mem {
    int offset;
};
/// Represent a immediate value.
struct Imm {
    int val;
};
using Operand = std::variant<Mem, Imm>;

struct Generator {
    /// The entry function.
    std::string operator()(pc::Token& token) {
        this->_gen(token);
        std::string ret = prelude;

        auto chars = main_prologue(this->_stack_size);
        ret += chars.get();

        for (auto& p: this->_body) ((ret += '\t') += p.get()) += '\n';

        chars = main_epilogue(this->_stack_size);
        ret += chars.get();

        return ret;
    }

  private:
    int _stack_size = 32;
    std::unordered_map<std::string_view, int> _offset_map{};
    std::vector<Chars> _body{};

    /// Process statements and generate assembly.
    void _gen(pc::Token& token) {
        switch (token.tag()) {
        case program.tag: {
            for (auto& child: token.children) this->_gen(child);
            break;
        }
        case read_stmt.tag: {
            for (auto& child: token.children) {
                auto val = std::get<Mem>(this->_eval(child));
                // invoke function
                this->_add_instr("jal __micro_read");
                // write return value to memory
                this->_add_instr("sw $v0, %d($s8)", val.offset);
            }
            break;
        }
        case write_stmt.tag: {
            for (auto& child: token.children) {
                // set arguments
                this->_add_instr(this->_eval(child), "lw $a0, %d($s8)", "li $a0, %d");
                // invoke function
                this->_add_instr("jal __micro_write");
            }
            break;
        }
        case assign_stmt.tag: {
            auto lhs = std::get<Mem>(this->_eval(token.children[0]));
            // fetch rhs
            this->_add_instr(this->_eval(token.children[1]), "lw $t0, %d($s8)", "li $t0, %d");
            // write to lhs
            this->_add_instr("sw $t0, %d($s8)", lhs.offset);
            break;
        }
        default: {
            throw std::domain_error("Unexpected tag!");
        }
        }
    }

    /// Process expressions and generate assembly. Return the operand that points to the expression.
    Operand _eval(pc::Token& token) {
        switch (token.tag()) {
        case expr.tag: {
            auto iter = token.children.begin();
            auto ret = _eval(*iter++);

            for (; iter != token.children.end();) {
                char op = *iter++->begin();
                auto next = _eval(*iter++);

                // fetch lhs
                this->_add_instr(ret, "lw $t0, %d($s8)", "li $t0, %d");
                // fetch rhs
                this->_add_instr(next, "lw $t1, %d($s8)", "li $t1, %d");
                // calculate
                this->_add_instr(op == '+' ? "add $t0, $t0, $t1" : "sub $t0, $t0, $t1");
                // write back
                this->_add_instr("sw $t0, %d($s8)", this->_stack_size);

                ret = Mem{this->_stack_size};
                this->_stack_size += 4;
            }

            return ret;
        }
        case ident.tag: {
            if (!this->_offset_map.contains(token.view())) {
                this->_offset_map[token.view()] = this->_stack_size;
                this->_stack_size += 4;
            }
            return Mem{this->_offset_map[token.view()]};
        }
        case integer.tag: {
            auto iter = token.begin();
            bool is_positive = *iter != '-';
            if (*iter == '-' || *iter == '+') ++iter;
            int ret = 0;
            for (; iter != token.end(); ++iter) ret = ret * 10 + (*iter - '0');
            return Imm{is_positive ? ret : -ret};
        }
        // // unreachable!
        // case add_op.tag:
        //     break;
        default:
            throw std::domain_error("Unexpected tag!");
        }
    }

    /// Add an instruction to `this->_body`.
    template<class... Args>
    void _add_instr(const char* s, Args... args) {
        this->_body.push_back(format(s, args...));
    }

    /// A convenient overload that handles `Operand`s.
    void _add_instr(Operand v, const char* s1, const char* s2) {
        this->_body.push_back(match(
            v,
            [s1](Mem mem) { return format(s1, mem.offset); },
            [s2](Imm imm) { return format(s2, imm.val); }));
    }
};

}  // namespace detail

/// A wrapper of `detail::Generator`.
std::string codegen(pc::Token& token) {
    return detail::Generator{}(token);
}
