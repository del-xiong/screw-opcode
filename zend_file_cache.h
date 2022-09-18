/*
   +----------------------------------------------------------------------+
   | Screw Opcode                                                         |
   +----------------------------------------------------------------------+

*/

#ifndef ZEND_FILE_CACHE_H
#define ZEND_FILE_CACHE_H

int zend_file_cache_script_store(zend_persistent_script *script, int in_shm);

zend_persistent_script *zend_file_cache_script_load(zend_file_handle *file_handle);

void zend_file_cache_invalidate(zend_string *full_path);

uint32_t screw_aes_128(int crypt, char *buf, uint8_t *key, int dataLen);

#endif /* ZEND_FILE_CACHE_H */
