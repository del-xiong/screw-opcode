/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+

*/

#define HAVE_OPROFILE 1

#include <opagent.h>

static op_agent_t op_agent = NULL;

static void zend_jit_oprofile_register(const char *name,
                                       const void *start,
                                       size_t size) {
    if (op_agent) {
        op_write_native_code(op_agent, name, (uint64_t)(zend_uintptr_t)
        start, start, size);
    }
}

static int zend_jit_oprofile_startup(void) {
    op_agent = op_open_agent();
    if (!op_agent) {
        fprintf(stderr, "OpAgent initialization failed [%d]!\n", errno);
        return 0;
    }
    return 1;
}

static void zend_jit_oprofile_shutdown(void) {
    if (op_agent) {
//???		sleep(60);
        op_close_agent(op_agent);
    }
}
