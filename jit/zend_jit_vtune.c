/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+

*/

#define HAVE_VTUNE 1

#include "jit/vtune/jitprofiling.h"
#include "jit/vtune/jitprofiling.c"

static void zend_jit_vtune_register(const char *name,
                                    const void *start,
                                    size_t      size)
{
	iJIT_Method_Load jmethod = {0};

	if (iJIT_IsProfilingActive() != iJIT_SAMPLING_ON) {
		return;
	}

	jmethod.method_id = iJIT_GetNewMethodID();
	jmethod.method_name = (char*)name;
	jmethod.class_file_name = NULL;
	jmethod.source_file_name = NULL;
	jmethod.method_load_address = (void*)start;
	jmethod.method_size = size;

	iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, (void*)&jmethod);
}
