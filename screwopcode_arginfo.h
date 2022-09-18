/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 06a37b4af6a0fb5d7bd24b1660ea8ce09e959ec8 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_screwopcode_reset,
0, 0, _IS_BOOL, 0)

ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_screwopcode_get_status,
0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, include_scripts, _IS_BOOL, 0, "true")

ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_screwopcode_compile_file,
0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)

ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_screwopcode_invalidate,
0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, force, _IS_BOOL, 0, "false")

ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_screwopcode_get_configuration,
0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)

ZEND_END_ARG_INFO()

#define arginfo_screwopcode_is_script_cached arginfo_screwopcode_compile_file


ZEND_FUNCTION(screwopcode_reset);
ZEND_FUNCTION(screwopcode_get_status);
ZEND_FUNCTION(screwopcode_compile_file);
ZEND_FUNCTION(screwopcode_invalidate);
ZEND_FUNCTION(screwopcode_get_configuration);
ZEND_FUNCTION(screwopcode_is_script_cached);


static const zend_function_entry ext_functions[] = {
        ZEND_FE(screwopcode_reset, arginfo_screwopcode_reset)
        ZEND_FE(screwopcode_get_status, arginfo_screwopcode_get_status)
        ZEND_FE(screwopcode_compile_file, arginfo_screwopcode_compile_file)
        ZEND_FE(screwopcode_invalidate, arginfo_screwopcode_invalidate)
        ZEND_FE(screwopcode_get_configuration, arginfo_screwopcode_get_configuration)
        ZEND_FE(screwopcode_is_script_cached, arginfo_screwopcode_is_script_cached)
        ZEND_FE_END
};