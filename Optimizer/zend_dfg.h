/*
   +----------------------------------------------------------------------+
   | Zend Engine, DFG - Data Flow Graph                                   |
   +----------------------------------------------------------------------+

*/

#ifndef ZEND_DFG_H
#define ZEND_DFG_H

#include "zend_bitset.h"
#include "zend_cfg.h"

typedef struct _zend_dfg {
    int vars;
    uint32_t size;
    zend_bitset tmp;
    zend_bitset def;
    zend_bitset use;
    zend_bitset in;
    zend_bitset out;
} zend_dfg;

#define DFG_BITSET(set, set_size, block_num) \
    ((set) + ((block_num) * (set_size)))

#define DFG_SET(set, set_size, block_num, var_num) \
    zend_bitset_incl(DFG_BITSET(set, set_size, block_num), (var_num))

#define DFG_ISSET(set, set_size, block_num, var_num) \
    zend_bitset_in(DFG_BITSET(set, set_size, block_num), (var_num))

BEGIN_EXTERN_C()

int zend_build_dfg(const zend_op_array *op_array, const zend_cfg *cfg, zend_dfg *dfg, uint32_t build_flags);

void
zend_dfg_add_use_def_op(const zend_op_array *op_array, const zend_op *opline, uint32_t build_flags, zend_bitset use,
                        zend_bitset def);

END_EXTERN_C()

#endif /* ZEND_DFG_H */
