/*
   Screw Opcode
*/

/* pass 4
 * - optimize INIT_FCALL_BY_NAME to DO_FCALL
 */

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

#define ZEND_OP1_IS_CONST_STRING(opline) \
	(opline->op1_type == IS_CONST &&     \
	 Z_TYPE(op_array->literals[(opline)->op1.constant]) == IS_STRING)
#define ZEND_OP2_IS_CONST_STRING(opline) \
	(opline->op2_type == IS_CONST &&     \
	 Z_TYPE(op_array->literals[(opline)->op2.constant]) == IS_STRING)

typedef struct _optimizer_call_info
{
	zend_function *func;
	zend_op *opline;
	zend_bool try_inline;
} optimizer_call_info;

static void zend_delete_call_instructions(zend_op *opline)
{
	int call = 0;

	while (1)
	{
		switch (opline->opcode)
		{
		case ZEND_INIT_FCALL_BY_NAME:
		case ZEND_INIT_NS_FCALL_BY_NAME:
		case ZEND_INIT_STATIC_METHOD_CALL:
		case ZEND_INIT_METHOD_CALL:
		case ZEND_INIT_FCALL:
			if (call == 0)
			{
				MAKE_NOP(opline);
				return;
			}
			/* break missing intentionally */
		case ZEND_NEW:
		case ZEND_INIT_DYNAMIC_CALL:
		case ZEND_INIT_USER_CALL:
			call--;
			break;
		case ZEND_DO_FCALL:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
		case ZEND_DO_FCALL_BY_NAME:
			call++;
			break;
		case ZEND_SEND_VAL:
		case ZEND_SEND_VAR:
			if (call == 0)
			{
				if (opline->op1_type == IS_CONST)
				{
					MAKE_NOP(opline);
				}
				else if (opline->op1_type == IS_CV)
				{
					opline->opcode = ZEND_CHECK_VAR;
					opline->extended_value = 0;
					opline->result.var = 0;
				}
				else
				{
					opline->opcode = ZEND_FREE;
					opline->extended_value = 0;
					opline->result.var = 0;
				}
			}
			break;
		}
		opline--;
	}
}

static void zend_try_inline_call(zend_op_array *op_array, zend_op *fcall, zend_op *opline, zend_function *func)
{
	if (func->type == ZEND_USER_FUNCTION && !(func->op_array.fn_flags & (ZEND_ACC_ABSTRACT | ZEND_ACC_HAS_TYPE_HINTS)) && fcall->extended_value >= func->op_array.required_num_args && func->op_array.opcodes[func->op_array.num_args].opcode == ZEND_RETURN)
	{

		zend_op *ret_opline = func->op_array.opcodes + func->op_array.num_args;

		if (ret_opline->op1_type == IS_CONST)
		{
			uint32_t i, num_args = func->op_array.num_args;
			num_args += (func->op_array.fn_flags & ZEND_ACC_VARIADIC) != 0;

			if (fcall->opcode == ZEND_INIT_METHOD_CALL && fcall->op1_type == IS_UNUSED)
			{
				/* TODO: we can't inlne methods, because $this may be used
				 *       not in object context ???
				 */
				return;
			}

			for (i = 0; i < num_args; i++)
			{
				/* Don't inline functions with by-reference arguments. This would require
				 * correct handling of INDIRECT arguments. */
				if (func->op_array.arg_info[i].pass_by_reference)
				{
					return;
				}
			}

			if (fcall->extended_value < func->op_array.num_args)
			{
				/* don't inline funcions with named constants in default arguments */
				i = fcall->extended_value;

				do
				{
					if (Z_CONSTANT_P(RT_CONSTANT(&func->op_array, func->op_array.opcodes[i].op2)))
					{
						return;
					}
					i++;
				} while (i < func->op_array.num_args);
			}

			if (RETURN_VALUE_USED(opline))
			{
				zval zv;

				ZVAL_DUP(&zv, RT_CONSTANT(&func->op_array, ret_opline->op1));
				opline->opcode = ZEND_QM_ASSIGN;
				opline->op1_type = IS_CONST;
				opline->op1.constant = zend_optimizer_add_literal(op_array, &zv);
				SET_UNUSED(opline->op2);
			}
			else
			{
				MAKE_NOP(opline);
			}

			zend_delete_call_instructions(opline - 1);
		}
	}
}

void zend_optimize_func_calls(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	int call = 0;
	void *checkpoint;
	optimizer_call_info *call_stack;

	if (op_array->last < 2)
	{
		return;
	}

	checkpoint = zend_arena_checkpoint(ctx->arena);
	call_stack = zend_arena_calloc(&ctx->arena, op_array->last / 2, sizeof(optimizer_call_info));
	while (opline < end)
	{
		switch (opline->opcode)
		{
		case ZEND_INIT_FCALL_BY_NAME:
		case ZEND_INIT_NS_FCALL_BY_NAME:
		case ZEND_INIT_STATIC_METHOD_CALL:
		case ZEND_INIT_METHOD_CALL:
		case ZEND_INIT_FCALL:
		case ZEND_NEW:
			call_stack[call].func = zend_optimizer_get_called_func(
				ctx->script, op_array, opline, 0);
			call_stack[call].try_inline = opline->opcode != ZEND_NEW;
			/* break missing intentionally */
		case ZEND_INIT_DYNAMIC_CALL:
		case ZEND_INIT_USER_CALL:
			call_stack[call].opline = opline;
			call++;
			break;
		case ZEND_DO_FCALL:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
		case ZEND_DO_FCALL_BY_NAME:
			call--;
			if (call_stack[call].func && call_stack[call].opline)
			{
				zend_op *fcall = call_stack[call].opline;

				if (fcall->opcode == ZEND_INIT_FCALL)
				{
					/* nothing to do */
				}
				else if (fcall->opcode == ZEND_INIT_FCALL_BY_NAME)
				{
					fcall->opcode = ZEND_INIT_FCALL;
					fcall->op1.num = zend_vm_calc_used_stack(fcall->extended_value, call_stack[call].func);
					Z_CACHE_SLOT(op_array->literals[fcall->op2.constant + 1]) = Z_CACHE_SLOT(op_array->literals[fcall->op2.constant]);
					literal_dtor(&ZEND_OP2_LITERAL(fcall));
					fcall->op2.constant = fcall->op2.constant + 1;
					opline->opcode = zend_get_call_op(fcall, call_stack[call].func);
				}
				else if (fcall->opcode == ZEND_INIT_NS_FCALL_BY_NAME)
				{
					fcall->opcode = ZEND_INIT_FCALL;
					fcall->op1.num = zend_vm_calc_used_stack(fcall->extended_value, call_stack[call].func);
					Z_CACHE_SLOT(op_array->literals[fcall->op2.constant + 1]) = Z_CACHE_SLOT(op_array->literals[fcall->op2.constant]);
					literal_dtor(&op_array->literals[fcall->op2.constant]);
					literal_dtor(&op_array->literals[fcall->op2.constant + 2]);
					fcall->op2.constant = fcall->op2.constant + 1;
					opline->opcode = zend_get_call_op(fcall, call_stack[call].func);
				}
				else if (fcall->opcode == ZEND_INIT_STATIC_METHOD_CALL || fcall->opcode == ZEND_INIT_METHOD_CALL || fcall->opcode == ZEND_NEW)
				{
					/* We don't have specialized opcodes for this, do nothing */
				}
				else
				{
					ZEND_ASSERT(0);
				}

				if ((ZEND_OPTIMIZER_PASS_16 & ctx->optimization_level) && call_stack[call].try_inline)
				{
					zend_try_inline_call(op_array, fcall, opline, call_stack[call].func);
				}
			}
			call_stack[call].func = NULL;
			call_stack[call].opline = NULL;
			call_stack[call].try_inline = 0;
			break;
		case ZEND_FETCH_FUNC_ARG:
		case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
		case ZEND_FETCH_OBJ_FUNC_ARG:
		case ZEND_FETCH_DIM_FUNC_ARG:
			if (call_stack[call - 1].func)
			{
				if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, (opline->extended_value & ZEND_FETCH_ARG_MASK)))
				{
					opline->extended_value &= ZEND_FETCH_TYPE_MASK;
					if (opline->opcode != ZEND_FETCH_STATIC_PROP_FUNC_ARG)
					{
						opline->opcode -= 9;
					}
					else
					{
						opline->opcode = ZEND_FETCH_STATIC_PROP_W;
					}
				}
				else
				{
					if (opline->opcode == ZEND_FETCH_DIM_FUNC_ARG && opline->op2_type == IS_UNUSED)
					{
						/* FETCH_DIM_FUNC_ARG supports UNUSED op2, while FETCH_DIM_R does not.
						 * Performing the replacement would create an invalid opcode. */
						call_stack[call - 1].try_inline = 0;
						break;
					}

					opline->extended_value &= ZEND_FETCH_TYPE_MASK;
					if (opline->opcode != ZEND_FETCH_STATIC_PROP_FUNC_ARG)
					{
						opline->opcode -= 12;
					}
					else
					{
						opline->opcode = ZEND_FETCH_STATIC_PROP_R;
					}
				}
			}
			break;
		case ZEND_SEND_VAL_EX:
			if (call_stack[call - 1].func)
			{
				if (ARG_MUST_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num))
				{
					/* We won't convert it into_DO_FCALL to emit error at run-time */
					call_stack[call - 1].opline = NULL;
				}
				else
				{
					opline->opcode = ZEND_SEND_VAL;
				}
			}
			break;
		case ZEND_SEND_VAR_EX:
			if (call_stack[call - 1].func)
			{
				if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num))
				{
					opline->opcode = ZEND_SEND_REF;
				}
				else
				{
					opline->opcode = ZEND_SEND_VAR;
				}
			}
			break;
		case ZEND_SEND_VAR_NO_REF_EX:
			if (call_stack[call - 1].func)
			{
				if (ARG_MUST_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num))
				{
					opline->opcode = ZEND_SEND_VAR_NO_REF;
				}
				else if (ARG_MAY_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num))
				{
					opline->opcode = ZEND_SEND_VAL;
				}
				else
				{
					opline->opcode = ZEND_SEND_VAR;
				}
			}
			break;
		case ZEND_SEND_UNPACK:
		case ZEND_SEND_USER:
		case ZEND_SEND_ARRAY:
			call_stack[call - 1].try_inline = 0;
			break;
		default:
			break;
		}
		opline++;
	}

	zend_arena_release(&ctx->arena, checkpoint);
}
