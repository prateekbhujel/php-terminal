/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 60f8f753765f0022a192ce679d342351424faa9c */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Io_Terminal_get_backend, 0, 0, Io\\Terminal\\Backend, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Io_Terminal_is_tty, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_Io_Terminal_supports_ansi arginfo_Io_Terminal_is_tty

#define arginfo_Io_Terminal_enable_ansi arginfo_Io_Terminal_is_tty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_Io_Terminal_get_size, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_Io_Terminal_get_width, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_Io_Terminal_get_height arginfo_Io_Terminal_get_width

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Io_Terminal_get_color_depth, 0, 0, Io\\Terminal\\ColorDepth, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Io_Terminal_supports_color, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, depth, Io\\Terminal\\ColorDepth, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_Io_Terminal_supports_true_color arginfo_Io_Terminal_is_tty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Io_Terminal_set_title, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, title, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_Io_Terminal_beep arginfo_Io_Terminal_is_tty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_Io_Terminal_write, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_Io_Terminal_enable_raw_mode, 0, 0, Io\\Terminal\\ModeToken, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Io_Terminal_restore_mode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mode, Io\\Terminal\\ModeToken, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_Io_Terminal_read_key, 0, 0, Io\\Terminal\\Key, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sequenceTimeout, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Io_Terminal_read_secret, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prompt, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_ColorDepth_bits, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Terminal_ModeToken___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Terminal_Terminal___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_stdin, 0, 0, Io\\Terminal\\Terminal, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_stdout arginfo_class_Io_Terminal_Terminal_stdin

#define arginfo_class_Io_Terminal_Terminal_stderr arginfo_class_Io_Terminal_Terminal_stdin

#define arginfo_class_Io_Terminal_Terminal_getBackend arginfo_Io_Terminal_get_backend

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_getStream, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_isTty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_supportsAnsi arginfo_class_Io_Terminal_Terminal_isTty

#define arginfo_class_Io_Terminal_Terminal_enableAnsi arginfo_class_Io_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Io_Terminal_Terminal_getSize, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Io_Terminal_Terminal_getWidth, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_getHeight arginfo_class_Io_Terminal_Terminal_getWidth

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_getColorDepth, 0, 0, Io\\Terminal\\ColorDepth, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_supportsColor, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, depth, Io\\Terminal\\ColorDepth, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_supportsTrueColor arginfo_class_Io_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_setTitle, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, title, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_beep arginfo_class_Io_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Io_Terminal_Terminal_write, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Io_Terminal_Terminal_enableRawMode, 0, 0, Io\\Terminal\\ModeToken, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_restoreMode, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, mode, Io\\Terminal\\ModeToken, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Io_Terminal_Terminal_readKey, 0, 0, Io\\Terminal\\Key, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sequenceTimeout, IS_DOUBLE, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_readSecret, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prompt, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_Terminal_Terminal_getBackend arginfo_Io_Terminal_get_backend

#define arginfo_class_Terminal_Terminal_isTty arginfo_Io_Terminal_is_tty

#define arginfo_class_Terminal_Terminal_supportsAnsi arginfo_Io_Terminal_is_tty

#define arginfo_class_Terminal_Terminal_enableAnsi arginfo_Io_Terminal_is_tty

#define arginfo_class_Terminal_Terminal_getSize arginfo_Io_Terminal_get_size

#define arginfo_class_Terminal_Terminal_getWidth arginfo_Io_Terminal_get_width

#define arginfo_class_Terminal_Terminal_getHeight arginfo_Io_Terminal_get_width

#define arginfo_class_Terminal_Terminal_getColorDepth arginfo_Io_Terminal_get_color_depth

#define arginfo_class_Terminal_Terminal_supportsColor arginfo_Io_Terminal_supports_color

#define arginfo_class_Terminal_Terminal_supportsTrueColor arginfo_Io_Terminal_is_tty

#define arginfo_class_Terminal_Terminal_setTitle arginfo_Io_Terminal_set_title

#define arginfo_class_Terminal_Terminal_beep arginfo_Io_Terminal_is_tty

#define arginfo_class_Terminal_Terminal_write arginfo_Io_Terminal_write

#define arginfo_class_Terminal_Terminal_enableRawMode arginfo_Io_Terminal_enable_raw_mode

#define arginfo_class_Terminal_Terminal_restoreMode arginfo_Io_Terminal_restore_mode

#define arginfo_class_Terminal_Terminal_readKey arginfo_class_Io_Terminal_Terminal_readKey

#define arginfo_class_Terminal_Terminal_readSecret arginfo_class_Io_Terminal_Terminal_readSecret

ZEND_FUNCTION(Io_Terminal_get_backend);
ZEND_FUNCTION(Io_Terminal_is_tty);
ZEND_FUNCTION(Io_Terminal_supports_ansi);
ZEND_FUNCTION(Io_Terminal_enable_ansi);
ZEND_FUNCTION(Io_Terminal_get_size);
ZEND_FUNCTION(Io_Terminal_get_width);
ZEND_FUNCTION(Io_Terminal_get_height);
ZEND_FUNCTION(Io_Terminal_get_color_depth);
ZEND_FUNCTION(Io_Terminal_supports_color);
ZEND_FUNCTION(Io_Terminal_supports_true_color);
ZEND_FUNCTION(Io_Terminal_set_title);
ZEND_FUNCTION(Io_Terminal_beep);
ZEND_FUNCTION(Io_Terminal_write);
ZEND_FUNCTION(Io_Terminal_enable_raw_mode);
ZEND_FUNCTION(Io_Terminal_restore_mode);
ZEND_FUNCTION(Io_Terminal_read_key);
ZEND_FUNCTION(Io_Terminal_read_secret);
ZEND_METHOD(Io_Terminal_ColorDepth, bits);
ZEND_METHOD(Io_Terminal_ModeToken, __construct);
ZEND_METHOD(Io_Terminal_Terminal, __construct);
ZEND_METHOD(Io_Terminal_Terminal, stdin);
ZEND_METHOD(Io_Terminal_Terminal, stdout);
ZEND_METHOD(Io_Terminal_Terminal, stderr);
ZEND_METHOD(Io_Terminal_Terminal, getBackend);
ZEND_METHOD(Io_Terminal_Terminal, getStream);
ZEND_METHOD(Io_Terminal_Terminal, isTty);
ZEND_METHOD(Io_Terminal_Terminal, supportsAnsi);
ZEND_METHOD(Io_Terminal_Terminal, enableAnsi);
ZEND_METHOD(Io_Terminal_Terminal, getSize);
ZEND_METHOD(Io_Terminal_Terminal, getWidth);
ZEND_METHOD(Io_Terminal_Terminal, getHeight);
ZEND_METHOD(Io_Terminal_Terminal, getColorDepth);
ZEND_METHOD(Io_Terminal_Terminal, supportsColor);
ZEND_METHOD(Io_Terminal_Terminal, supportsTrueColor);
ZEND_METHOD(Io_Terminal_Terminal, setTitle);
ZEND_METHOD(Io_Terminal_Terminal, beep);
ZEND_METHOD(Io_Terminal_Terminal, write);
ZEND_METHOD(Io_Terminal_Terminal, enableRawMode);
ZEND_METHOD(Io_Terminal_Terminal, restoreMode);
ZEND_METHOD(Io_Terminal_Terminal, readKey);
ZEND_METHOD(Io_Terminal_Terminal, readSecret);
ZEND_METHOD(Terminal_Terminal, getBackend);
ZEND_METHOD(Terminal_Terminal, isTty);
ZEND_METHOD(Terminal_Terminal, supportsAnsi);
ZEND_METHOD(Terminal_Terminal, enableAnsi);
ZEND_METHOD(Terminal_Terminal, getSize);
ZEND_METHOD(Terminal_Terminal, getWidth);
ZEND_METHOD(Terminal_Terminal, getHeight);
ZEND_METHOD(Terminal_Terminal, getColorDepth);
ZEND_METHOD(Terminal_Terminal, supportsColor);
ZEND_METHOD(Terminal_Terminal, supportsTrueColor);
ZEND_METHOD(Terminal_Terminal, setTitle);
ZEND_METHOD(Terminal_Terminal, beep);
ZEND_METHOD(Terminal_Terminal, write);
ZEND_METHOD(Terminal_Terminal, enableRawMode);
ZEND_METHOD(Terminal_Terminal, restoreMode);
ZEND_METHOD(Terminal_Terminal, readKey);
ZEND_METHOD(Terminal_Terminal, readSecret);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "get_backend"), zif_Io_Terminal_get_backend, arginfo_Io_Terminal_get_backend, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "is_tty"), zif_Io_Terminal_is_tty, arginfo_Io_Terminal_is_tty, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "supports_ansi"), zif_Io_Terminal_supports_ansi, arginfo_Io_Terminal_supports_ansi, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "enable_ansi"), zif_Io_Terminal_enable_ansi, arginfo_Io_Terminal_enable_ansi, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "get_size"), zif_Io_Terminal_get_size, arginfo_Io_Terminal_get_size, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "get_width"), zif_Io_Terminal_get_width, arginfo_Io_Terminal_get_width, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "get_height"), zif_Io_Terminal_get_height, arginfo_Io_Terminal_get_height, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "get_color_depth"), zif_Io_Terminal_get_color_depth, arginfo_Io_Terminal_get_color_depth, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "supports_color"), zif_Io_Terminal_supports_color, arginfo_Io_Terminal_supports_color, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "supports_true_color"), zif_Io_Terminal_supports_true_color, arginfo_Io_Terminal_supports_true_color, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "set_title"), zif_Io_Terminal_set_title, arginfo_Io_Terminal_set_title, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "beep"), zif_Io_Terminal_beep, arginfo_Io_Terminal_beep, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "write"), zif_Io_Terminal_write, arginfo_Io_Terminal_write, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "enable_raw_mode"), zif_Io_Terminal_enable_raw_mode, arginfo_Io_Terminal_enable_raw_mode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "restore_mode"), zif_Io_Terminal_restore_mode, arginfo_Io_Terminal_restore_mode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "read_key"), zif_Io_Terminal_read_key, arginfo_Io_Terminal_read_key, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Terminal", "read_secret"), zif_Io_Terminal_read_secret, arginfo_Io_Terminal_read_secret, 0, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_ColorDepth_methods[] = {
	ZEND_ME(Io_Terminal_ColorDepth, bits, arginfo_class_Io_Terminal_ColorDepth_bits, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_ModeToken_methods[] = {
	ZEND_ME(Io_Terminal_ModeToken, __construct, arginfo_class_Io_Terminal_ModeToken___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_Terminal_methods[] = {
	ZEND_ME(Io_Terminal_Terminal, __construct, arginfo_class_Io_Terminal_Terminal___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, stdin, arginfo_class_Io_Terminal_Terminal_stdin, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, stdout, arginfo_class_Io_Terminal_Terminal_stdout, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, stderr, arginfo_class_Io_Terminal_Terminal_stderr, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, getBackend, arginfo_class_Io_Terminal_Terminal_getBackend, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, getStream, arginfo_class_Io_Terminal_Terminal_getStream, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, isTty, arginfo_class_Io_Terminal_Terminal_isTty, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, supportsAnsi, arginfo_class_Io_Terminal_Terminal_supportsAnsi, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, enableAnsi, arginfo_class_Io_Terminal_Terminal_enableAnsi, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, getSize, arginfo_class_Io_Terminal_Terminal_getSize, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, getWidth, arginfo_class_Io_Terminal_Terminal_getWidth, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, getHeight, arginfo_class_Io_Terminal_Terminal_getHeight, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, getColorDepth, arginfo_class_Io_Terminal_Terminal_getColorDepth, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, supportsColor, arginfo_class_Io_Terminal_Terminal_supportsColor, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, supportsTrueColor, arginfo_class_Io_Terminal_Terminal_supportsTrueColor, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, setTitle, arginfo_class_Io_Terminal_Terminal_setTitle, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, beep, arginfo_class_Io_Terminal_Terminal_beep, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, write, arginfo_class_Io_Terminal_Terminal_write, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, enableRawMode, arginfo_class_Io_Terminal_Terminal_enableRawMode, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, restoreMode, arginfo_class_Io_Terminal_Terminal_restoreMode, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, readKey, arginfo_class_Io_Terminal_Terminal_readKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, readSecret, arginfo_class_Io_Terminal_Terminal_readSecret, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Terminal_Terminal_methods[] = {
	ZEND_ME(Terminal_Terminal, getBackend, arginfo_class_Terminal_Terminal_getBackend, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, isTty, arginfo_class_Terminal_Terminal_isTty, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, supportsAnsi, arginfo_class_Terminal_Terminal_supportsAnsi, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, enableAnsi, arginfo_class_Terminal_Terminal_enableAnsi, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, getSize, arginfo_class_Terminal_Terminal_getSize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, getWidth, arginfo_class_Terminal_Terminal_getWidth, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, getHeight, arginfo_class_Terminal_Terminal_getHeight, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, getColorDepth, arginfo_class_Terminal_Terminal_getColorDepth, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, supportsColor, arginfo_class_Terminal_Terminal_supportsColor, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, supportsTrueColor, arginfo_class_Terminal_Terminal_supportsTrueColor, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, setTitle, arginfo_class_Terminal_Terminal_setTitle, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, beep, arginfo_class_Terminal_Terminal_beep, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, write, arginfo_class_Terminal_Terminal_write, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, enableRawMode, arginfo_class_Terminal_Terminal_enableRawMode, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, restoreMode, arginfo_class_Terminal_Terminal_restoreMode, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, readKey, arginfo_class_Terminal_Terminal_readKey, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, readSecret, arginfo_class_Terminal_Terminal_readSecret, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Io_Terminal_Backend(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Terminal\\Backend", IS_UNDEF, NULL);
	zend_register_class_alias("Terminal\\Backend", class_entry);

	zend_enum_add_case_cstr(class_entry, "Posix", NULL);

	zend_enum_add_case_cstr(class_entry, "Windows", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_Stream(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Terminal\\Stream", IS_UNDEF, NULL);
	zend_register_class_alias("Terminal\\Stream", class_entry);

	zend_enum_add_case_cstr(class_entry, "Stdin", NULL);

	zend_enum_add_case_cstr(class_entry, "Stdout", NULL);

	zend_enum_add_case_cstr(class_entry, "Stderr", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_ColorDepth(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Terminal\\ColorDepth", IS_UNDEF, class_Io_Terminal_ColorDepth_methods);
	zend_register_class_alias("Terminal\\ColorDepth", class_entry);

	zend_enum_add_case_cstr(class_entry, "None", NULL);

	zend_enum_add_case_cstr(class_entry, "Standard", NULL);

	zend_enum_add_case_cstr(class_entry, "Extended", NULL);

	zend_enum_add_case_cstr(class_entry, "TrueColor", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_Key(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Terminal\\Key", IS_UNDEF, NULL);
	zend_register_class_alias("Terminal\\Key", class_entry);

	zend_enum_add_case_cstr(class_entry, "Up", NULL);

	zend_enum_add_case_cstr(class_entry, "Down", NULL);

	zend_enum_add_case_cstr(class_entry, "Right", NULL);

	zend_enum_add_case_cstr(class_entry, "Left", NULL);

	zend_enum_add_case_cstr(class_entry, "Enter", NULL);

	zend_enum_add_case_cstr(class_entry, "Backspace", NULL);

	zend_enum_add_case_cstr(class_entry, "Escape", NULL);

	zend_enum_add_case_cstr(class_entry, "Tab", NULL);

	zend_enum_add_case_cstr(class_entry, "Home", NULL);

	zend_enum_add_case_cstr(class_entry, "End", NULL);

	zend_enum_add_case_cstr(class_entry, "Delete", NULL);

	zend_enum_add_case_cstr(class_entry, "PageUp", NULL);

	zend_enum_add_case_cstr(class_entry, "PageDown", NULL);

	zend_enum_add_case_cstr(class_entry, "Resize", NULL);

	zend_enum_add_case_cstr(class_entry, "F1", NULL);

	zend_enum_add_case_cstr(class_entry, "F2", NULL);

	zend_enum_add_case_cstr(class_entry, "F3", NULL);

	zend_enum_add_case_cstr(class_entry, "F4", NULL);

	zend_enum_add_case_cstr(class_entry, "F5", NULL);

	zend_enum_add_case_cstr(class_entry, "F6", NULL);

	zend_enum_add_case_cstr(class_entry, "F7", NULL);

	zend_enum_add_case_cstr(class_entry, "F8", NULL);

	zend_enum_add_case_cstr(class_entry, "F9", NULL);

	zend_enum_add_case_cstr(class_entry, "F10", NULL);

	zend_enum_add_case_cstr(class_entry, "F11", NULL);

	zend_enum_add_case_cstr(class_entry, "F12", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_ModeToken(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Terminal", "ModeToken", class_Io_Terminal_ModeToken_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_register_class_alias("Terminal\\ModeToken", class_entry);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_Terminal(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Terminal", "Terminal", class_Io_Terminal_Terminal_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);

	return class_entry;
}

static zend_class_entry *register_class_Terminal_Terminal(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Terminal", "Terminal", class_Terminal_Terminal_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	return class_entry;
}
