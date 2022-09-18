/*
   +----------------------------------------------------------------------+
   | Screw Opcode                                                         |
   +----------------------------------------------------------------------+

*/

#ifndef ZEND_PERSIST_H
#define ZEND_PERSIST_H

uint32_t
zend_accel_script_persist_calc(zend_persistent_script *script, const char *key, unsigned int key_length, int for_shm);

zend_persistent_script *
zend_accel_script_persist(zend_persistent_script *script, const char **key, unsigned int key_length, int for_shm);

#endif /* ZEND_PERSIST_H */
