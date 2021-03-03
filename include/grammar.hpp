#pragma once

#include "qcpc/qcpc.hpp"

// clang-format off

QCPC_DECL(expr);

QCPC_DECL_DEF(sep)
  = *one<' ', '\t', '\r', '\n'>
  ;

QCPC_DECL_DEF(integer)
  = -one<'-'> & +range<'0', '9'>
  ;
QCPC_DECL_DEF(ident)
  = range<'a', 'z', 'A', 'Z'> & *range<'a', 'z', 'A', 'Z', '0', '9'>
  ;
QCPC_DECL_DEF_(primary)
  = join(sep, one<'('>, expr, one<')'>)
  | ident
  | integer
  ;

QCPC_DECL_DEF(add_op)
  = one<'+', '-'>
  ;
QCPC_DEF(expr)
  = list(primary, add_op, sep)
  ;
QCPC_DECL_DEF_(expr_list)
  = list(expr, one<','>, sep)
  ;
QCPC_DECL_DEF_(id_list)
  = list(ident, one<','>, sep)
  ;

QCPC_DECL_DEF(assign_stmt)
  = join(sep, ident, str<':', '='>, expr, one<';'>)
  ;
QCPC_DECL_DEF(read_stmt)
  = join(sep, str<'r', 'e', 'a', 'd'>, one<'('>, id_list, one<')'>, one<';'>)
  ;
QCPC_DECL_DEF(write_stmt)
  = join(sep, str<'w', 'r', 'i', 't', 'e'>, one<'('>, expr_list, one<')'>, one<';'>)
  ;
QCPC_DECL_DEF_(stmt)
  = assign_stmt
  | read_stmt
  | write_stmt
  ;

QCPC_DECL_DEF_(program)
  = boi & join(sep, str<'b', 'e', 'g', 'i', 'n'>, +stmt, str<'e', 'n', 'd'>) & soi
  ;

// clang-format on
