/*
   +----------------------------------------------------------------------+
   | Zend Engine, Bytecode Visualisation                                  |
   +----------------------------------------------------------------------+

*/

#ifndef ZEND_DUMP_H
#define ZEND_DUMP_H

#include "zend_ssa.h"
#include "zend_dfg.h"

#define ZEND_DUMP_HIDE_UNREACHABLE     (1<<0)
#define ZEND_DUMP_RC_INFERENCE         (1<<1)
#define ZEND_DUMP_CFG                  (1<<2)
#define ZEND_DUMP_SSA                  (1<<3)
#define ZEND_DUMP_LIVE_RANGES          (1<<4)

BEGIN_EXTERN_C()

void zend_dump_op_array(const zend_op_array *op_array, uint32_t dump_flags, const char *msg, const void *data);

void zend_dump_op(const zend_op_array *op_array, const zend_basic_block *b, const zend_op *opline, uint32_t dump_flags,
                  const zend_ssa *ssa, const zend_ssa_op *ssa_op);

void zend_dump_dominators(const zend_op_array *op_array, const zend_cfg *cfg);

void zend_dump_dfg(const zend_op_array *op_array, const zend_cfg *cfg, const zend_dfg *dfg);

void zend_dump_phi_placement(const zend_op_array *op_array, const zend_ssa *ssa);

void zend_dump_variables(const zend_op_array *op_array);

void zend_dump_ssa_variables(const zend_op_array *op_array, const zend_ssa *ssa, uint32_t dump_flags);

void
zend_dump_ssa_var(const zend_op_array *op_array, const zend_ssa *ssa, int ssa_var_num, zend_uchar var_type, int var_num,
                  uint32_t dump_flags);

void zend_dump_var(const zend_op_array *op_array, zend_uchar var_type, int var_num);

void zend_dump_op_array_name(const zend_op_array *op_array);

void zend_dump_const(const zval *zv);

void zend_dump_ht(HashTable *ht);

END_EXTERN_C()

#endif /* ZEND_DUMP_H */
