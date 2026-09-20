/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e72cd5162435c5ff36d8a1d982929d97deca397f */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_ColorDepth_bits, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Terminal_ModeToken___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Terminal_TerminalSize___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, cols, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, rows, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_TerminalSize_toArray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Terminal_Terminal___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, output, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_create, 0, 0, Io\\Terminal\\Terminal, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_open arginfo_class_Io_Terminal_Terminal_create

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_fromStreams, 0, 1, Io\\Terminal\\Terminal, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, output, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_fromStream, 0, 1, Io\\Terminal\\Terminal, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_getBackend, 0, 0, Io\\Terminal\\Backend, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_getInputStream, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_getOutputStream arginfo_class_Io_Terminal_Terminal_getInputStream

#define arginfo_class_Io_Terminal_Terminal_getStream arginfo_class_Io_Terminal_Terminal_getInputStream

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_isTty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal_supportsAnsi arginfo_class_Io_Terminal_Terminal_isTty

#define arginfo_class_Io_Terminal_Terminal_enableAnsi arginfo_class_Io_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Io_Terminal_Terminal_getSize, 0, 0, Io\\Terminal\\TerminalSize, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

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

#define arginfo_class_Terminal_Terminal_getBackend arginfo_class_Io_Terminal_Terminal_getBackend

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Terminal_Terminal_isTty, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Terminal_Terminal_supportsAnsi arginfo_class_Terminal_Terminal_isTty

#define arginfo_class_Terminal_Terminal_enableAnsi arginfo_class_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_getSize, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_getWidth, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Terminal_Terminal_getHeight arginfo_class_Terminal_Terminal_getWidth

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Terminal_Terminal_getColorDepth, 0, 0, Io\\Terminal\\ColorDepth, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Terminal_Terminal_supportsColor, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, depth, Io\\Terminal\\ColorDepth, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Terminal_Terminal_supportsTrueColor arginfo_class_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Terminal_Terminal_setTitle, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, title, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Terminal_Terminal_beep arginfo_class_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_write, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_enableRawMode, 0, 0, Io\\Terminal\\ModeToken, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, stream, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Terminal_Terminal_restoreMode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mode, Io\\Terminal\\ModeToken, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Terminal_Terminal_readKey arginfo_class_Io_Terminal_Terminal_readKey

#define arginfo_class_Terminal_Terminal_readSecret arginfo_class_Io_Terminal_Terminal_readSecret

ZEND_METHOD(Io_Terminal_ColorDepth, bits);
ZEND_METHOD(Io_Terminal_ModeToken, __construct);
ZEND_METHOD(Io_Terminal_TerminalSize, __construct);
ZEND_METHOD(Io_Terminal_TerminalSize, toArray);
ZEND_METHOD(Io_Terminal_Terminal, __construct);
ZEND_METHOD(Io_Terminal_Terminal, create);
ZEND_METHOD(Io_Terminal_Terminal, open);
ZEND_METHOD(Io_Terminal_Terminal, fromStreams);
ZEND_METHOD(Io_Terminal_Terminal, fromStream);
ZEND_METHOD(Io_Terminal_Terminal, getBackend);
ZEND_METHOD(Io_Terminal_Terminal, getInputStream);
ZEND_METHOD(Io_Terminal_Terminal, getOutputStream);
ZEND_METHOD(Io_Terminal_Terminal, getStream);
ZEND_METHOD(Io_Terminal_Terminal, isTty);
ZEND_METHOD(Io_Terminal_Terminal, supportsAnsi);
ZEND_METHOD(Io_Terminal_Terminal, enableAnsi);
ZEND_METHOD(Io_Terminal_Terminal, getSize);
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

static const zend_function_entry class_Io_Terminal_ColorDepth_methods[] = {
	ZEND_ME(Io_Terminal_ColorDepth, bits, arginfo_class_Io_Terminal_ColorDepth_bits, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_ModeToken_methods[] = {
	ZEND_ME(Io_Terminal_ModeToken, __construct, arginfo_class_Io_Terminal_ModeToken___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_TerminalSize_methods[] = {
	ZEND_ME(Io_Terminal_TerminalSize, __construct, arginfo_class_Io_Terminal_TerminalSize___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_TerminalSize, toArray, arginfo_class_Io_Terminal_TerminalSize_toArray, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_Terminal_methods[] = {
	ZEND_ME(Io_Terminal_Terminal, __construct, arginfo_class_Io_Terminal_Terminal___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, create, arginfo_class_Io_Terminal_Terminal_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, open, arginfo_class_Io_Terminal_Terminal_open, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, fromStreams, arginfo_class_Io_Terminal_Terminal_fromStreams, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, fromStream, arginfo_class_Io_Terminal_Terminal_fromStream, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, getBackend, arginfo_class_Io_Terminal_Terminal_getBackend, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, getInputStream, arginfo_class_Io_Terminal_Terminal_getInputStream, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, getOutputStream, arginfo_class_Io_Terminal_Terminal_getOutputStream, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, getStream, arginfo_class_Io_Terminal_Terminal_getStream, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, isTty, arginfo_class_Io_Terminal_Terminal_isTty, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, supportsAnsi, arginfo_class_Io_Terminal_Terminal_supportsAnsi, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, enableAnsi, arginfo_class_Io_Terminal_Terminal_enableAnsi, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, getSize, arginfo_class_Io_Terminal_Terminal_getSize, ZEND_ACC_PUBLIC)
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

static zend_class_entry *register_class_Io_Terminal_TerminalSize(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Terminal", "TerminalSize", class_Io_Terminal_TerminalSize_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
	zend_register_class_alias("Terminal\\TerminalSize", class_entry);

	zval property_cols_default_value;
	ZVAL_UNDEF(&property_cols_default_value);
	zend_string *property_cols_name = zend_string_init("cols", sizeof("cols") - 1, 1);
	zend_declare_typed_property(class_entry, property_cols_name, &property_cols_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_cols_name);

	zval property_rows_default_value;
	ZVAL_UNDEF(&property_rows_default_value);
	zend_string *property_rows_name = zend_string_init("rows", sizeof("rows") - 1, 1);
	zend_declare_typed_property(class_entry, property_rows_name, &property_rows_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_rows_name);

	zval property_width_default_value;
	ZVAL_UNDEF(&property_width_default_value);
	zend_string *property_width_name = zend_string_init("width", sizeof("width") - 1, 1);
	zend_declare_typed_property(class_entry, property_width_name, &property_width_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_width_name);

	zval property_height_default_value;
	ZVAL_UNDEF(&property_height_default_value);
	zend_string *property_height_name = zend_string_init("height", sizeof("height") - 1, 1);
	zend_declare_typed_property(class_entry, property_height_name, &property_height_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_height_name);

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
