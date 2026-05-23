/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2bc60c2cd1580fb37a8c97b52c69a6ca97832529 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_terminal_backend, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_terminal_is_tty, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stream, IS_LONG, 0, "TERMINAL_STDOUT")
ZEND_END_ARG_INFO()

#define arginfo_terminal_supports_ansi arginfo_terminal_is_tty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_terminal_get_size, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stream, IS_LONG, 0, "TERMINAL_STDOUT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_terminal_write, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stream, IS_LONG, 0, "TERMINAL_STDOUT")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(terminal_backend);
ZEND_FUNCTION(terminal_is_tty);
ZEND_FUNCTION(terminal_supports_ansi);
ZEND_FUNCTION(terminal_get_size);
ZEND_FUNCTION(terminal_write);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(terminal_backend, arginfo_terminal_backend)
	ZEND_FE(terminal_is_tty, arginfo_terminal_is_tty)
	ZEND_FE(terminal_supports_ansi, arginfo_terminal_supports_ansi)
	ZEND_FE(terminal_get_size, arginfo_terminal_get_size)
	ZEND_FE(terminal_write, arginfo_terminal_write)
	ZEND_FE_END
};
