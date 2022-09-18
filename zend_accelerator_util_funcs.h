/*
   +----------------------------------------------------------------------+
   | Screw Opcode (based on Zend Opcache)                                 
   +----------------------------------------------------------------------+

*/

#ifndef ZEND_ACCELERATOR_UTIL_FUNCS_H
#define ZEND_ACCELERATOR_UTIL_FUNCS_H

#include "zend.h"
#include "ZendAccelerator.h"

zend_persistent_script *create_persistent_script(void);

void free_persistent_script(zend_persistent_script *persistent_script, int destroy_elements);

void zend_accel_move_user_functions(HashTable *str, uint32_t count, zend_script *script);

void zend_accel_move_user_classes(HashTable *str, uint32_t count, zend_script *script);

zend_op_array *zend_accel_load_script(zend_persistent_script *persistent_script, int from_shared_memory);

#define ADLER32_INIT 1     /* initial Adler-32 value */

unsigned int zend_adler32(unsigned int checksum, unsigned char *buf, uint32_t len);

unsigned int zend_accel_script_checksum(zend_persistent_script *persistent_script);

#endif /* ZEND_ACCELERATOR_UTIL_FUNCS_H */
