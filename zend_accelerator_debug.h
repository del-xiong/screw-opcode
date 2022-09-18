/*
   +----------------------------------------------------------------------+
   | Screw Opcode (based on Zend Opcache)                                 
   +----------------------------------------------------------------------+

*/

#ifndef ZEND_ACCELERATOR_DEBUG_H
#define ZEND_ACCELERATOR_DEBUG_H

#define ACCEL_LOG_FATAL                    0
#define ACCEL_LOG_ERROR                    1
#define ACCEL_LOG_WARNING                2
#define ACCEL_LOG_INFO                    3
#define ACCEL_LOG_DEBUG                    4

void zend_accel_error(int type, const char *format, ...)

ZEND_ATTRIBUTE_FORMAT(printf,
2, 3);

#endif /* _ZEND_ACCELERATOR_DEBUG_H */
