/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2fb950155c5a708276c397a66ee24752e8c8feda */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Terminal_ModeToken___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Terminal_Terminal_getBackend, 0, 0, Terminal\\Backend, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Terminal_Terminal_isTty, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, stream, "Terminal\\Stream::Stdout")
ZEND_END_ARG_INFO()

#define arginfo_class_Terminal_Terminal_supportsAnsi arginfo_class_Terminal_Terminal_isTty

#define arginfo_class_Terminal_Terminal_enableAnsi arginfo_class_Terminal_Terminal_isTty

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_getSize, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, stream, "Terminal\\Stream::Stdout")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_write, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, stream, "Terminal\\Stream::Stdout")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_enableRawMode, 0, 0, Terminal\\ModeToken, MAY_BE_FALSE)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, stream, "Terminal\\Stream::Stdin")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Terminal_Terminal_restoreMode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mode, Terminal\\ModeToken, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Terminal_Terminal_readKey, 0, 0, Terminal\\Key, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sequenceTimeout, IS_DOUBLE, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Terminal_Terminal_readSecret, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prompt, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_METHOD(Terminal_ModeToken, __construct);
ZEND_METHOD(Terminal_Terminal, getBackend);
ZEND_METHOD(Terminal_Terminal, isTty);
ZEND_METHOD(Terminal_Terminal, supportsAnsi);
ZEND_METHOD(Terminal_Terminal, enableAnsi);
ZEND_METHOD(Terminal_Terminal, getSize);
ZEND_METHOD(Terminal_Terminal, write);
ZEND_METHOD(Terminal_Terminal, enableRawMode);
ZEND_METHOD(Terminal_Terminal, restoreMode);
ZEND_METHOD(Terminal_Terminal, readKey);
ZEND_METHOD(Terminal_Terminal, readSecret);

static const zend_function_entry class_Terminal_ModeToken_methods[] = {
	ZEND_ME(Terminal_ModeToken, __construct, arginfo_class_Terminal_ModeToken___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_Terminal_Terminal_methods[] = {
	ZEND_ME(Terminal_Terminal, getBackend, arginfo_class_Terminal_Terminal_getBackend, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, isTty, arginfo_class_Terminal_Terminal_isTty, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, supportsAnsi, arginfo_class_Terminal_Terminal_supportsAnsi, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, enableAnsi, arginfo_class_Terminal_Terminal_enableAnsi, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, getSize, arginfo_class_Terminal_Terminal_getSize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, write, arginfo_class_Terminal_Terminal_write, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, enableRawMode, arginfo_class_Terminal_Terminal_enableRawMode, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, restoreMode, arginfo_class_Terminal_Terminal_restoreMode, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, readKey, arginfo_class_Terminal_Terminal_readKey, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Terminal_Terminal, readSecret, arginfo_class_Terminal_Terminal_readSecret, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Terminal_Backend(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Terminal\\Backend", IS_STRING, NULL);

	zval enum_case_Posix_value;
	zend_string *enum_case_Posix_value_str = zend_string_init("posix", strlen("posix"), 1);
	ZVAL_STR(&enum_case_Posix_value, enum_case_Posix_value_str);
	zend_enum_add_case_cstr(class_entry, "Posix", &enum_case_Posix_value);

	zval enum_case_Windows_value;
	zend_string *enum_case_Windows_value_str = zend_string_init("windows", strlen("windows"), 1);
	ZVAL_STR(&enum_case_Windows_value, enum_case_Windows_value_str);
	zend_enum_add_case_cstr(class_entry, "Windows", &enum_case_Windows_value);

	return class_entry;
}

static zend_class_entry *register_class_Terminal_Stream(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Terminal\\Stream", IS_LONG, NULL);

	zval enum_case_Stdin_value;
	ZVAL_LONG(&enum_case_Stdin_value, 0);
	zend_enum_add_case_cstr(class_entry, "Stdin", &enum_case_Stdin_value);

	zval enum_case_Stdout_value;
	ZVAL_LONG(&enum_case_Stdout_value, 1);
	zend_enum_add_case_cstr(class_entry, "Stdout", &enum_case_Stdout_value);

	zval enum_case_Stderr_value;
	ZVAL_LONG(&enum_case_Stderr_value, 2);
	zend_enum_add_case_cstr(class_entry, "Stderr", &enum_case_Stderr_value);

	return class_entry;
}

static zend_class_entry *register_class_Terminal_Key(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Terminal\\Key", IS_STRING, NULL);

	zval enum_case_Up_value;
	zend_string *enum_case_Up_value_str = zend_string_init("up", strlen("up"), 1);
	ZVAL_STR(&enum_case_Up_value, enum_case_Up_value_str);
	zend_enum_add_case_cstr(class_entry, "Up", &enum_case_Up_value);

	zval enum_case_Down_value;
	zend_string *enum_case_Down_value_str = zend_string_init("down", strlen("down"), 1);
	ZVAL_STR(&enum_case_Down_value, enum_case_Down_value_str);
	zend_enum_add_case_cstr(class_entry, "Down", &enum_case_Down_value);

	zval enum_case_Right_value;
	zend_string *enum_case_Right_value_str = zend_string_init("right", strlen("right"), 1);
	ZVAL_STR(&enum_case_Right_value, enum_case_Right_value_str);
	zend_enum_add_case_cstr(class_entry, "Right", &enum_case_Right_value);

	zval enum_case_Left_value;
	zend_string *enum_case_Left_value_str = zend_string_init("left", strlen("left"), 1);
	ZVAL_STR(&enum_case_Left_value, enum_case_Left_value_str);
	zend_enum_add_case_cstr(class_entry, "Left", &enum_case_Left_value);

	zval enum_case_Enter_value;
	zend_string *enum_case_Enter_value_str = zend_string_init("enter", strlen("enter"), 1);
	ZVAL_STR(&enum_case_Enter_value, enum_case_Enter_value_str);
	zend_enum_add_case_cstr(class_entry, "Enter", &enum_case_Enter_value);

	zval enum_case_Backspace_value;
	zend_string *enum_case_Backspace_value_str = zend_string_init("backspace", strlen("backspace"), 1);
	ZVAL_STR(&enum_case_Backspace_value, enum_case_Backspace_value_str);
	zend_enum_add_case_cstr(class_entry, "Backspace", &enum_case_Backspace_value);

	zval enum_case_Escape_value;
	zend_string *enum_case_Escape_value_str = zend_string_init("escape", strlen("escape"), 1);
	ZVAL_STR(&enum_case_Escape_value, enum_case_Escape_value_str);
	zend_enum_add_case_cstr(class_entry, "Escape", &enum_case_Escape_value);

	zval enum_case_Tab_value;
	zend_string *enum_case_Tab_value_str = zend_string_init("tab", strlen("tab"), 1);
	ZVAL_STR(&enum_case_Tab_value, enum_case_Tab_value_str);
	zend_enum_add_case_cstr(class_entry, "Tab", &enum_case_Tab_value);

	zval enum_case_Home_value;
	zend_string *enum_case_Home_value_str = zend_string_init("home", strlen("home"), 1);
	ZVAL_STR(&enum_case_Home_value, enum_case_Home_value_str);
	zend_enum_add_case_cstr(class_entry, "Home", &enum_case_Home_value);

	zval enum_case_End_value;
	zend_string *enum_case_End_value_str = zend_string_init("end", strlen("end"), 1);
	ZVAL_STR(&enum_case_End_value, enum_case_End_value_str);
	zend_enum_add_case_cstr(class_entry, "End", &enum_case_End_value);

	zval enum_case_Delete_value;
	zend_string *enum_case_Delete_value_str = zend_string_init("delete", strlen("delete"), 1);
	ZVAL_STR(&enum_case_Delete_value, enum_case_Delete_value_str);
	zend_enum_add_case_cstr(class_entry, "Delete", &enum_case_Delete_value);

	zval enum_case_PageUp_value;
	zend_string *enum_case_PageUp_value_str = zend_string_init("pageup", strlen("pageup"), 1);
	ZVAL_STR(&enum_case_PageUp_value, enum_case_PageUp_value_str);
	zend_enum_add_case_cstr(class_entry, "PageUp", &enum_case_PageUp_value);

	zval enum_case_PageDown_value;
	zend_string *enum_case_PageDown_value_str = zend_string_init("pagedown", strlen("pagedown"), 1);
	ZVAL_STR(&enum_case_PageDown_value, enum_case_PageDown_value_str);
	zend_enum_add_case_cstr(class_entry, "PageDown", &enum_case_PageDown_value);

	zval enum_case_Resize_value;
	zend_string *enum_case_Resize_value_str = zend_string_init("resize", strlen("resize"), 1);
	ZVAL_STR(&enum_case_Resize_value, enum_case_Resize_value_str);
	zend_enum_add_case_cstr(class_entry, "Resize", &enum_case_Resize_value);

	zval enum_case_F1_value;
	zend_string *enum_case_F1_value_str = zend_string_init("f1", strlen("f1"), 1);
	ZVAL_STR(&enum_case_F1_value, enum_case_F1_value_str);
	zend_enum_add_case_cstr(class_entry, "F1", &enum_case_F1_value);

	zval enum_case_F2_value;
	zend_string *enum_case_F2_value_str = zend_string_init("f2", strlen("f2"), 1);
	ZVAL_STR(&enum_case_F2_value, enum_case_F2_value_str);
	zend_enum_add_case_cstr(class_entry, "F2", &enum_case_F2_value);

	zval enum_case_F3_value;
	zend_string *enum_case_F3_value_str = zend_string_init("f3", strlen("f3"), 1);
	ZVAL_STR(&enum_case_F3_value, enum_case_F3_value_str);
	zend_enum_add_case_cstr(class_entry, "F3", &enum_case_F3_value);

	zval enum_case_F4_value;
	zend_string *enum_case_F4_value_str = zend_string_init("f4", strlen("f4"), 1);
	ZVAL_STR(&enum_case_F4_value, enum_case_F4_value_str);
	zend_enum_add_case_cstr(class_entry, "F4", &enum_case_F4_value);

	zval enum_case_F5_value;
	zend_string *enum_case_F5_value_str = zend_string_init("f5", strlen("f5"), 1);
	ZVAL_STR(&enum_case_F5_value, enum_case_F5_value_str);
	zend_enum_add_case_cstr(class_entry, "F5", &enum_case_F5_value);

	zval enum_case_F6_value;
	zend_string *enum_case_F6_value_str = zend_string_init("f6", strlen("f6"), 1);
	ZVAL_STR(&enum_case_F6_value, enum_case_F6_value_str);
	zend_enum_add_case_cstr(class_entry, "F6", &enum_case_F6_value);

	zval enum_case_F7_value;
	zend_string *enum_case_F7_value_str = zend_string_init("f7", strlen("f7"), 1);
	ZVAL_STR(&enum_case_F7_value, enum_case_F7_value_str);
	zend_enum_add_case_cstr(class_entry, "F7", &enum_case_F7_value);

	zval enum_case_F8_value;
	zend_string *enum_case_F8_value_str = zend_string_init("f8", strlen("f8"), 1);
	ZVAL_STR(&enum_case_F8_value, enum_case_F8_value_str);
	zend_enum_add_case_cstr(class_entry, "F8", &enum_case_F8_value);

	zval enum_case_F9_value;
	zend_string *enum_case_F9_value_str = zend_string_init("f9", strlen("f9"), 1);
	ZVAL_STR(&enum_case_F9_value, enum_case_F9_value_str);
	zend_enum_add_case_cstr(class_entry, "F9", &enum_case_F9_value);

	zval enum_case_F10_value;
	zend_string *enum_case_F10_value_str = zend_string_init("f10", strlen("f10"), 1);
	ZVAL_STR(&enum_case_F10_value, enum_case_F10_value_str);
	zend_enum_add_case_cstr(class_entry, "F10", &enum_case_F10_value);

	zval enum_case_F11_value;
	zend_string *enum_case_F11_value_str = zend_string_init("f11", strlen("f11"), 1);
	ZVAL_STR(&enum_case_F11_value, enum_case_F11_value_str);
	zend_enum_add_case_cstr(class_entry, "F11", &enum_case_F11_value);

	zval enum_case_F12_value;
	zend_string *enum_case_F12_value_str = zend_string_init("f12", strlen("f12"), 1);
	ZVAL_STR(&enum_case_F12_value, enum_case_F12_value_str);
	zend_enum_add_case_cstr(class_entry, "F12", &enum_case_F12_value);

	return class_entry;
}

static zend_class_entry *register_class_Terminal_ModeToken(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Terminal", "ModeToken", class_Terminal_ModeToken_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_Terminal_Terminal(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Terminal", "Terminal", class_Terminal_Terminal_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
