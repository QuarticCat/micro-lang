#pragma once

#include "qcpc/qcpc.hpp"

namespace pc = qcpc;

#define S(str) QCPC_STR(str)

// clang-format off

QCPC_DECL(expr);

QCPC_DECL_DEF_(sep)
  = *pc::one<' ', '\t', '\r', '\n'>
  ;

QCPC_DECL_DEF(integer)
  = -S("-") & +pc::range<'0', '9'>
  ;
QCPC_DECL_DEF(ident)
  = pc::range<'a', 'z', 'A', 'Z'> & *pc::range<'a', 'z', 'A', 'Z', '0', '9'>
  ;
QCPC_DECL_DEF_(primary)
  = pc::join(sep, S("("), expr, S(")"))
  | ident
  | integer
  ;

QCPC_DECL_DEF(add_op)
  = pc::one<'+', '-'>
  ;
QCPC_DEF(expr)
  = pc::list(primary, add_op, sep)
  ;
QCPC_DECL_DEF_(expr_list)
  = pc::list(expr, S(","), sep)
  ;
QCPC_DECL_DEF_(id_list)
  = pc::list(ident, S(","), sep)
  ;

QCPC_DECL_DEF(assign_stmt)
  = pc::join(sep, ident, S(":="), expr, S(";"))
  ;
QCPC_DECL_DEF(read_stmt)
  = pc::join(sep, S("read"), S("("), id_list, S(")"), S(";"))
  ;
QCPC_DECL_DEF(write_stmt)
  = pc::join(sep, S("write"), S("("), expr_list, S(")"), S(";"))
  ;
QCPC_DECL_DEF_(stmt)
  = assign_stmt
  | read_stmt
  | write_stmt
  ;

QCPC_DECL_DEF(program)
  = pc::join(sep, pc::boi, S("begin"), *stmt, S("end"), pc::eoi)
  ;

// clang-format on

#undef S
