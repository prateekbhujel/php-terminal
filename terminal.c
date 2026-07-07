/* terminal extension for PHP (c) 2026 Pratik Bhujel */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_enum.h"
#include "main/php_network.h"
#include "main/php_streams.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/info.h"
#include "php_terminal.h"
#include "terminal_arginfo.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef PHP_WIN32
# include <io.h>
# include <windows.h>
#else
# include <signal.h>
# include <sys/select.h>
# include <sys/time.h>
# include <termios.h>
# include <time.h>
# include <unistd.h>
# include <sys/ioctl.h>
#endif

#define TERMINAL_MODE_TOKEN_MAGIC "PHPTTY1"
#define TERMINAL_MODE_TOKEN_MAGIC_LEN (sizeof(TERMINAL_MODE_TOKEN_MAGIC) - 1)
#define TERMINAL_SEQUENCE_TIMEOUT_MS 25

#ifdef PHP_WIN32
typedef HANDLE terminal_native_stream;
#else
typedef int terminal_native_stream;
#endif

typedef struct _terminal_saved_mode {
	char magic[TERMINAL_MODE_TOKEN_MAGIC_LEN];
	terminal_native_stream stream;
	bool marks_standard_input;
#ifdef PHP_WIN32
	DWORD mode;
#else
	struct termios mode;
#endif
} terminal_saved_mode;

typedef struct _terminal_mode_token_object {
	terminal_saved_mode saved;
	zval stream_resource;
	bool valid;
	zend_object std;
} terminal_mode_token_object;

static zend_class_entry *terminal_backend_ce;
static zend_class_entry *terminal_stream_ce;
static zend_class_entry *terminal_key_ce;
static zend_class_entry *terminal_mode_token_ce;
static zend_object_handlers terminal_mode_token_handlers;

static inline terminal_mode_token_object *terminal_mode_token_from_obj(zend_object *obj)
{
	return (terminal_mode_token_object *) ((char *) obj - offsetof(terminal_mode_token_object, std));
}

#define Z_TERMINAL_MODE_TOKEN_P(zv) terminal_mode_token_from_obj(Z_OBJ_P(zv))

static zend_object *terminal_mode_token_create_object(zend_class_entry *ce)
{
	terminal_mode_token_object *intern = zend_object_alloc(sizeof(terminal_mode_token_object), ce);

	memset(&intern->saved, 0, sizeof(intern->saved));
	ZVAL_UNDEF(&intern->stream_resource);
	intern->valid = false;

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = &terminal_mode_token_handlers;

	return &intern->std;
}

static void terminal_mode_token_free_obj(zend_object *object)
{
	terminal_mode_token_object *intern = terminal_mode_token_from_obj(object);

	if (!Z_ISUNDEF(intern->stream_resource)) {
		zval_ptr_dtor(&intern->stream_resource);
		ZVAL_UNDEF(&intern->stream_resource);
	}

	memset(&intern->saved, 0, sizeof(intern->saved));
	intern->valid = false;

	zend_object_std_dtor(&intern->std);
}

static void terminal_create_mode_token(zval *return_value, const terminal_saved_mode *saved, zval *stream_resource)
{
	terminal_mode_token_object *intern;

	object_init_ex(return_value, terminal_mode_token_ce);
	intern = Z_TERMINAL_MODE_TOKEN_P(return_value);
	intern->saved = *saved;
	if (stream_resource != NULL) {
		ZVAL_COPY(&intern->stream_resource, stream_resource);
	}
	intern->valid = true;
}

static zend_string *terminal_key_string(const char *key)
{
	return zend_string_init(key, strlen(key), false);
}

static zend_string *terminal_key_char(unsigned char key)
{
	return zend_string_init((const char *) &key, 1, false);
}

static zend_long terminal_stream_from_enum(zval *stream)
{
	return Z_LVAL_P(zend_enum_fetch_case_value(Z_OBJ_P(stream)));
}

static void terminal_set_enum_case(zval *return_value, zend_class_entry *ce, const char *case_name)
{
	RETVAL_OBJ_COPY(zend_enum_get_case_cstr(ce, case_name));
}

static zend_object *terminal_key_enum_from_string(zend_string *key)
{
	if (zend_string_equals_literal(key, "up")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Up");
	}
	if (zend_string_equals_literal(key, "down")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Down");
	}
	if (zend_string_equals_literal(key, "right")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Right");
	}
	if (zend_string_equals_literal(key, "left")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Left");
	}
	if (zend_string_equals_literal(key, "enter")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Enter");
	}
	if (zend_string_equals_literal(key, "backspace")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Backspace");
	}
	if (zend_string_equals_literal(key, "escape")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Escape");
	}
	if (zend_string_equals_literal(key, "tab")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Tab");
	}
	if (zend_string_equals_literal(key, "home")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Home");
	}
	if (zend_string_equals_literal(key, "end")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "End");
	}
	if (zend_string_equals_literal(key, "delete")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Delete");
	}
	if (zend_string_equals_literal(key, "pageup")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "PageUp");
	}
	if (zend_string_equals_literal(key, "pagedown")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "PageDown");
	}
	if (zend_string_equals_literal(key, "resize")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "Resize");
	}
	if (zend_string_equals_literal(key, "f1")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F1");
	}
	if (zend_string_equals_literal(key, "f2")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F2");
	}
	if (zend_string_equals_literal(key, "f3")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F3");
	}
	if (zend_string_equals_literal(key, "f4")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F4");
	}
	if (zend_string_equals_literal(key, "f5")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F5");
	}
	if (zend_string_equals_literal(key, "f6")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F6");
	}
	if (zend_string_equals_literal(key, "f7")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F7");
	}
	if (zend_string_equals_literal(key, "f8")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F8");
	}
	if (zend_string_equals_literal(key, "f9")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F9");
	}
	if (zend_string_equals_literal(key, "f10")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F10");
	}
	if (zend_string_equals_literal(key, "f11")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F11");
	}
	if (zend_string_equals_literal(key, "f12")) {
		return zend_enum_get_case_cstr(terminal_key_ce, "F12");
	}

	return NULL;
}

static bool terminal_buffer_remove_last_utf8_char(smart_str *buffer)
{
	size_t len;

	if (buffer->s == NULL || ZSTR_LEN(buffer->s) == 0) {
		return false;
	}

	len = ZSTR_LEN(buffer->s);

	do {
		len--;
	} while (len > 0 && (((unsigned char) ZSTR_VAL(buffer->s)[len]) & 0xc0) == 0x80);

	ZSTR_VAL(buffer->s)[len] = '\0';
	ZSTR_LEN(buffer->s) = len;

	return true;
}

static bool terminal_parse_positive_env_long(zend_string *value, zend_long *result)
{
	const char *cursor;
	zend_long parsed = 0;

	if (value == NULL || ZSTR_LEN(value) == 0) {
		return false;
	}

	for (cursor = ZSTR_VAL(value); *cursor != '\0'; cursor++) {
		zend_long digit;

		if (*cursor < '0' || *cursor > '9') {
			return false;
		}

		digit = (zend_long) (*cursor - '0');
		if (parsed > (ZEND_LONG_MAX - digit) / 10) {
			return false;
		}

		parsed = (parsed * 10) + digit;
	}

	*result = (zend_long) parsed;

	return parsed > 0;
}

static bool terminal_size_from_environment(zend_long *columns, zend_long *rows)
{
	zend_string *columns_value = php_getenv("COLUMNS", sizeof("COLUMNS") - 1);
	zend_string *rows_value = php_getenv("LINES", sizeof("LINES") - 1);
	zend_long env_columns;
	zend_long env_rows;
	bool result = false;

	if (terminal_parse_positive_env_long(columns_value, &env_columns)
		&& terminal_parse_positive_env_long(rows_value, &env_rows)) {
		*columns = env_columns;
		*rows = env_rows;
		result = true;
	}

	if (columns_value != NULL) {
		zend_string_release(columns_value);
	}
	if (rows_value != NULL) {
		zend_string_release(rows_value);
	}

	return result;
}

#ifdef PHP_WIN32
static bool terminal_env_is_non_empty(const char *name, size_t name_len)
{
	zend_string *value = php_getenv(name, name_len);
	bool result;

	if (value == NULL) {
		return false;
	}

	result = ZSTR_LEN(value) > 0;
	zend_string_release(value);

	return result;
}

static bool terminal_no_color_is_set(void)
{
	return terminal_env_is_non_empty("NO_COLOR", sizeof("NO_COLOR") - 1);
}

static int terminal_win_stdin_is_raw = 0;

static terminal_native_stream terminal_native_stream_from_id(zend_long stream)
{
	switch (stream) {
		case TERMINAL_STREAM_STDIN:
			return GetStdHandle(STD_INPUT_HANDLE);
		case TERMINAL_STREAM_STDOUT:
			return GetStdHandle(STD_OUTPUT_HANDLE);
		case TERMINAL_STREAM_STDERR:
			return GetStdHandle(STD_ERROR_HANDLE);
		default:
			return INVALID_HANDLE_VALUE;
	}
}

static terminal_native_stream terminal_native_stream_from_php_stream(php_stream *stream)
{
	php_socket_t descriptor = (php_socket_t) -1;
	intptr_t os_handle;

	if (php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void **) &descriptor, 0) != SUCCESS) {
		return INVALID_HANDLE_VALUE;
	}

	if (descriptor == INVALID_SOCKET || (uintptr_t) descriptor > INT_MAX) {
		return INVALID_HANDLE_VALUE;
	}

	os_handle = _get_osfhandle((int) descriptor);

	return os_handle == -1 ? INVALID_HANDLE_VALUE : (HANDLE) os_handle;
}

static bool terminal_native_stream_is_valid(terminal_native_stream stream)
{
	return stream != INVALID_HANDLE_VALUE && stream != NULL;
}

static bool terminal_stream_is_tty(terminal_native_stream handle)
{
	DWORD mode;

	return terminal_native_stream_is_valid(handle)
		&& GetConsoleMode(handle, &mode);
}

static bool terminal_stream_supports_ansi(terminal_native_stream handle)
{
	DWORD mode;
	DWORD ansi_mode;

	if (terminal_no_color_is_set()) {
		return false;
	}

	if (!terminal_native_stream_is_valid(handle)) {
		return false;
	}

	if (!GetConsoleMode(handle, &mode)) {
		return false;
	}

	if ((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0) {
		return true;
	}

	ansi_mode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(handle, ansi_mode)) {
		return false;
	}

	return SetConsoleMode(handle, mode);
}

static bool terminal_enable_stream_ansi(terminal_native_stream handle)
{
	DWORD mode;

	if (terminal_no_color_is_set()) {
		return false;
	}

	if (!terminal_native_stream_is_valid(handle)) {
		return false;
	}

	if (!GetConsoleMode(handle, &mode)) {
		return false;
	}

	if ((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0) {
		return true;
	}

	return SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

static bool terminal_stream_size(terminal_native_stream handle, zend_long *columns, zend_long *rows)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (!terminal_native_stream_is_valid(handle)) {
		return false;
	}

	if (!GetConsoleScreenBufferInfo(handle, &info)) {
		return false;
	}

	*columns = (zend_long) (info.srWindow.Right - info.srWindow.Left + 1);
	*rows = (zend_long) (info.srWindow.Bottom - info.srWindow.Top + 1);

	return true;
}

static bool terminal_stream_write(terminal_native_stream handle, const char *buffer, size_t buffer_len, zend_long *written)
{
	size_t total = 0;

	if (!terminal_native_stream_is_valid(handle)) {
		return false;
	}

	while (total < buffer_len) {
		DWORD chunk_len = buffer_len - total > MAXDWORD ? MAXDWORD : (DWORD) (buffer_len - total);
		DWORD chunk_written = 0;

		if (!WriteFile(handle, buffer + total, chunk_len, &chunk_written, NULL)) {
			return false;
		}

		if (chunk_written == 0) {
			return false;
		}

		total += (size_t) chunk_written;
	}

	*written = (zend_long) total;

	return true;
}

static DWORD terminal_make_raw_mode(DWORD mode, bool mark_stdin_raw)
{
	/* ReadConsoleInputW returns key events directly, so VT input is not required here. */
	if (mark_stdin_raw) {
		terminal_win_stdin_is_raw = 1;
	}

	return mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
}

static bool terminal_enable_stream_raw_mode(terminal_native_stream handle, bool marks_standard_input, terminal_saved_mode *saved)
{
	DWORD mode;
	DWORD raw_mode;

	if (!terminal_native_stream_is_valid(handle) || !GetConsoleMode(handle, &mode)) {
		return false;
	}

	raw_mode = terminal_make_raw_mode(mode, marks_standard_input);

	if (!SetConsoleMode(handle, raw_mode)) {
		if (marks_standard_input) {
			terminal_win_stdin_is_raw = 0;
		}
		return false;
	}

	memset(saved, 0, sizeof(*saved));
	memcpy(saved->magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = handle;
	saved->marks_standard_input = marks_standard_input;
	saved->mode = mode;

	return true;
}

static bool terminal_restore_stream_mode(const terminal_saved_mode *saved)
{
	bool restored = terminal_native_stream_is_valid(saved->stream)
		&& SetConsoleMode(saved->stream, saved->mode);

	if (restored && saved->marks_standard_input) {
		terminal_win_stdin_is_raw = 0;
	}

	return restored;
}

static DWORD terminal_timeout_to_wait_ms(double timeout, bool timeout_is_null)
{
	double timeout_ms;
	DWORD timeout_ms_dword;

	if (timeout_is_null) {
		return INFINITE;
	}

	if (timeout <= 0) {
		return 0;
	}

	timeout_ms = timeout * 1000;
	if (timeout_ms >= (double) (INFINITE - 1)) {
		return INFINITE - 1;
	}

	if (timeout_ms < 1) {
		return 1;
	}

	timeout_ms_dword = (DWORD) timeout_ms;

	return timeout_ms_dword < timeout_ms ? timeout_ms_dword + 1 : timeout_ms_dword;
}

static DWORD terminal_remaining_wait_ms(ULONGLONG deadline_ms)
{
	ULONGLONG now_ms = GetTickCount64();

	if (now_ms >= deadline_ms) {
		return 0;
	}

	if (deadline_ms - now_ms >= (ULONGLONG) (INFINITE - 1)) {
		return INFINITE - 1;
	}

	return (DWORD) (deadline_ms - now_ms);
}

static zend_string *terminal_key_from_virtual_key(WORD key)
{
	switch (key) {
		case VK_UP:
			return terminal_key_string("up");
		case VK_DOWN:
			return terminal_key_string("down");
		case VK_RIGHT:
			return terminal_key_string("right");
		case VK_LEFT:
			return terminal_key_string("left");
		case VK_RETURN:
			return terminal_key_string("enter");
		case VK_BACK:
			return terminal_key_string("backspace");
		case VK_ESCAPE:
			return terminal_key_string("escape");
		case VK_TAB:
			return terminal_key_string("tab");
		case VK_HOME:
			return terminal_key_string("home");
		case VK_END:
			return terminal_key_string("end");
		case VK_DELETE:
			return terminal_key_string("delete");
		case VK_PRIOR:
			return terminal_key_string("pageup");
		case VK_NEXT:
			return terminal_key_string("pagedown");
		case VK_F1:
			return terminal_key_string("f1");
		case VK_F2:
			return terminal_key_string("f2");
		case VK_F3:
			return terminal_key_string("f3");
		case VK_F4:
			return terminal_key_string("f4");
		case VK_F5:
			return terminal_key_string("f5");
		case VK_F6:
			return terminal_key_string("f6");
		case VK_F7:
			return terminal_key_string("f7");
		case VK_F8:
			return terminal_key_string("f8");
		case VK_F9:
			return terminal_key_string("f9");
		case VK_F10:
			return terminal_key_string("f10");
		case VK_F11:
			return terminal_key_string("f11");
		case VK_F12:
			return terminal_key_string("f12");
	}

	return NULL;
}

static zend_string *terminal_key_from_wchar(WCHAR key)
{
	char buffer[8];
	int buffer_len;

	if (key == L'\0') {
		return NULL;
	}

	if (key <= 0x7f) {
		return terminal_key_char((unsigned char) key);
	}

	buffer_len = WideCharToMultiByte(CP_UTF8, 0, &key, 1, buffer, sizeof(buffer), NULL, NULL);
	if (buffer_len <= 0) {
		return NULL;
	}

	return zend_string_init(buffer, (size_t) buffer_len, false);
}

static bool terminal_secret_append_wchar(smart_str *secret, WCHAR key)
{
	char buffer[8];
	int buffer_len;

	if (key == L'\0' || key < L' ') {
		return true;
	}

	if (key == 0x7f) {
		return true;
	}

	if (key <= 0x7f) {
		char byte = (char) key;
		smart_str_appendl(secret, &byte, 1);
		return true;
	}

	buffer_len = WideCharToMultiByte(CP_UTF8, 0, &key, 1, buffer, sizeof(buffer), NULL, NULL);
	if (buffer_len <= 0) {
		return false;
	}

	smart_str_appendl(secret, buffer, (size_t) buffer_len);

	return true;
}

static zend_string *terminal_key_from_input_record(const KEY_EVENT_RECORD *key)
{
	zend_string *named_key = terminal_key_from_virtual_key(key->wVirtualKeyCode);

	if (named_key != NULL) {
		return named_key;
	}

	return terminal_key_from_wchar(key->uChar.UnicodeChar);
}

static zend_string *terminal_read_stdin_key(double timeout, bool timeout_is_null, double sequence_timeout, bool sequence_timeout_is_null)
{
	HANDLE handle = terminal_native_stream_from_id(TERMINAL_STREAM_STDIN);
	DWORD mode = 0;
	DWORD raw_mode;
	DWORD wait_ms = terminal_timeout_to_wait_ms(timeout, timeout_is_null);
	ULONGLONG deadline_ms = wait_ms == INFINITE ? 0 : GetTickCount64() + wait_ms;
	zend_string *result = NULL;
	bool mode_changed = false;

	(void) sequence_timeout;
	(void) sequence_timeout_is_null;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
		return NULL;
	}

	/*
	 * Interactive prompt loops commonly call rawMode() once and readKey()
	 * many times. When stdin is already raw, read directly so one key read
	 * cannot restore the console to cooked mode mid-session.
	 */
	if (!terminal_win_stdin_is_raw) {
		if (!GetConsoleMode(handle, &mode)) {
			return NULL;
		}

		raw_mode = terminal_make_raw_mode(mode, false);
		if (raw_mode != mode && !SetConsoleMode(handle, raw_mode)) {
			return NULL;
		}
		mode_changed = raw_mode != mode;
	}

	for (;;) {
		INPUT_RECORD record;
		DWORD records_read;
		DWORD wait_result = WaitForSingleObject(handle, wait_ms);

		if (wait_result == WAIT_TIMEOUT) {
			break;
		}

		if (wait_result != WAIT_OBJECT_0) {
			break;
		}

		if (!ReadConsoleInputW(handle, &record, 1, &records_read) || records_read != 1) {
			break;
		}

		if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
			result = terminal_key_string("resize");
			break;
		}

		if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
			zend_string *key = terminal_key_from_input_record(&record.Event.KeyEvent);

			if (key != NULL) {
				result = key;
				break;
			}
		}

		if (!timeout_is_null) {
			wait_ms = terminal_remaining_wait_ms(deadline_ms);
			if (wait_ms == 0) {
				break;
			}
		}
	}

	if (mode_changed && !SetConsoleMode(handle, mode)) {
		if (result != NULL) {
			zend_string_release(result);
		}

		return NULL;
	}

	return result;
}

static zend_string *terminal_read_stdin_secret(void)
{
	HANDLE handle = terminal_native_stream_from_id(TERMINAL_STREAM_STDIN);
	DWORD mode;
	DWORD raw_mode;
	smart_str secret = {0};
	bool success = false;
	bool failed = false;
	bool mode_changed = false;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL || !GetConsoleMode(handle, &mode)) {
		return NULL;
	}

	raw_mode = terminal_make_raw_mode(mode, false);
	if (raw_mode != mode && !SetConsoleMode(handle, raw_mode)) {
		return NULL;
	}
	mode_changed = raw_mode != mode;

	for (;;) {
		INPUT_RECORD record;
		KEY_EVENT_RECORD *key;
		DWORD records_read;
		DWORD wait_result = WaitForSingleObject(handle, INFINITE);

		if (wait_result != WAIT_OBJECT_0) {
			failed = true;
			break;
		}

		if (!ReadConsoleInputW(handle, &record, 1, &records_read) || records_read != 1) {
			failed = true;
			break;
		}

		if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown) {
			continue;
		}

		key = &record.Event.KeyEvent;

		if (key->wVirtualKeyCode == VK_RETURN) {
			success = true;
			break;
		}

		if (key->wVirtualKeyCode == VK_BACK) {
			if (terminal_buffer_remove_last_utf8_char(&secret)) {
				zend_long written;

				terminal_stream_write(terminal_native_stream_from_id(TERMINAL_STREAM_STDOUT), "\b \b", sizeof("\b \b") - 1, &written);
			}
			continue;
		}

		if (key->wVirtualKeyCode == VK_ESCAPE || key->uChar.UnicodeChar == 0x03 || key->uChar.UnicodeChar == 0x04) {
			break;
		}

		{
			size_t before_len = secret.s == NULL ? 0 : ZSTR_LEN(secret.s);
			zend_long written;

			if (!terminal_secret_append_wchar(&secret, key->uChar.UnicodeChar)) {
				failed = true;
				break;
			}

			if ((secret.s == NULL ? 0 : ZSTR_LEN(secret.s)) > before_len) {
				terminal_stream_write(terminal_native_stream_from_id(TERMINAL_STREAM_STDOUT), "*", 1, &written);
			}
		}
	}

	if (mode_changed && !SetConsoleMode(handle, mode)) {
		smart_str_free(&secret);
		return NULL;
	}

	if (!success || failed) {
		smart_str_free(&secret);
		return NULL;
	}

	if (secret.s == NULL) {
		return zend_string_init("", 0, false);
	}

	smart_str_0(&secret);

	return secret.s;
}
#else
#define TERMINAL_READ_RESIZE 2

#ifdef SIGWINCH
static volatile sig_atomic_t terminal_resize_pending = 0;

static void terminal_sigwinch_handler(int signo)
{
	(void) signo;

	terminal_resize_pending = 1;
}

static bool terminal_install_resize_handler(struct sigaction *old_action)
{
	struct sigaction action;

	terminal_resize_pending = 0;

	memset(&action, 0, sizeof(action));
	action.sa_handler = terminal_sigwinch_handler;
	sigemptyset(&action.sa_mask);

	return sigaction(SIGWINCH, &action, old_action) == 0;
}

static void terminal_restore_resize_handler(const struct sigaction *old_action)
{
	sigaction(SIGWINCH, old_action, NULL);
}

static bool terminal_consume_resize_pending(void)
{
	if (terminal_resize_pending) {
		terminal_resize_pending = 0;
		return true;
	}

	return false;
}
#else
static bool terminal_install_resize_handler(void *old_action)
{
	(void) old_action;

	return false;
}

static void terminal_restore_resize_handler(const void *old_action)
{
	(void) old_action;
}

static bool terminal_consume_resize_pending(void)
{
	return false;
}
#endif

static terminal_native_stream terminal_native_stream_from_id(zend_long stream)
{
	switch (stream) {
		case TERMINAL_STREAM_STDIN:
			return STDIN_FILENO;
		case TERMINAL_STREAM_STDOUT:
			return STDOUT_FILENO;
		case TERMINAL_STREAM_STDERR:
			return STDERR_FILENO;
		default:
			return -1;
	}
}

static terminal_native_stream terminal_native_stream_from_php_stream(php_stream *stream)
{
	php_socket_t descriptor = (php_socket_t) -1;

	if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void **) &descriptor, 0) != SUCCESS
		&& php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void **) &descriptor, 0) != SUCCESS) {
		return -1;
	}

	if (descriptor < 0 || descriptor > INT_MAX) {
		return -1;
	}

	return (int) descriptor;
}

static bool terminal_native_stream_is_valid(terminal_native_stream stream)
{
	return stream >= 0;
}

static bool terminal_stream_is_tty(terminal_native_stream fd)
{
	return terminal_native_stream_is_valid(fd) && isatty(fd) == 1;
}

static const char *terminal_getenv_nonempty(const char *name)
{
	const char *value = getenv(name);

	return value != NULL && value[0] != '\0' ? value : NULL;
}

/*
 * ANSI capability detection order on Unix:
 * 1. NO_COLOR set in the environment disables ANSI output.
 *    Spec: https://no-color.org
 * 2. A non-TTY stream without COLORTERM is not ANSI capable.
 * 3. COLORTERM=truecolor or COLORTERM=24bit is ANSI capable.
 * 4. TERM unset or TERM=dumb is not ANSI capable.
 * 5. Known TERM_PROGRAM values are ANSI capable.
 * 6. TERM containing 256color or color is ANSI capable.
 * 7. Known color-capable TERM values are ANSI capable.
 * 8. Fall back to isatty(fd).
 */
static bool terminal_stream_supports_ansi(terminal_native_stream fd)
{
	bool is_tty;
	const char *colorterm;
	const char *term;
	const char *term_program;

	if (getenv("NO_COLOR") != NULL) {
		return false;
	}
	if (!terminal_native_stream_is_valid(fd)) {
		return false;
	}
	is_tty = isatty(fd) == 1;

	colorterm = terminal_getenv_nonempty("COLORTERM");
	if (!is_tty && colorterm == NULL) {
		return false;
	}

	if (colorterm != NULL && (strcmp(colorterm, "truecolor") == 0 || strcmp(colorterm, "24bit") == 0)) {
		return true;
	}

	term = terminal_getenv_nonempty("TERM");
	if (term == NULL || strcmp(term, "dumb") == 0) {
		return false;
	}

	term_program = terminal_getenv_nonempty("TERM_PROGRAM");
	if (term_program != NULL
		&& (strcmp(term_program, "iTerm.app") == 0
			|| strcmp(term_program, "Hyper") == 0
			|| strcmp(term_program, "WezTerm") == 0
			|| strcmp(term_program, "vscode") == 0
			|| strcmp(term_program, "Tabby") == 0)) {
		return true;
	}

	if (strstr(term, "256color") != NULL || strstr(term, "color") != NULL) {
		return true;
	}

	if (strcmp(term, "xterm") == 0
		|| strcmp(term, "rxvt") == 0
		|| strcmp(term, "screen") == 0
		|| strcmp(term, "tmux") == 0
		|| strcmp(term, "alacritty") == 0
		|| strcmp(term, "kitty") == 0) {
		return true;
	}

	return is_tty;
}

static bool terminal_enable_stream_ansi(terminal_native_stream stream)
{
	return terminal_stream_supports_ansi(stream);
}

static bool terminal_stream_size(terminal_native_stream fd, zend_long *columns, zend_long *rows)
{
	struct winsize ws;

	if (!terminal_native_stream_is_valid(fd)) {
		return false;
	}

	if (ioctl(fd, TIOCGWINSZ, &ws) != 0 || ws.ws_col == 0 || ws.ws_row == 0) {
		return false;
	}

	*columns = (zend_long) ws.ws_col;
	*rows = (zend_long) ws.ws_row;

	return true;
}

static bool terminal_stream_write(terminal_native_stream fd, const char *buffer, size_t buffer_len, zend_long *written)
{
	size_t total = 0;

	if (!terminal_native_stream_is_valid(fd)) {
		return false;
	}

	while (total < buffer_len) {
		ssize_t chunk = write(fd, buffer + total, buffer_len - total);

		if (chunk < 0) {
			if (errno == EINTR) {
				continue;
			}

			return false;
		}

		if (chunk == 0) {
			return false;
		}

		total += (size_t) chunk;
	}

	*written = (zend_long) total;

	return true;
}

static void terminal_make_raw_mode(struct termios *mode)
{
	tcflag_t lflag = ECHO | ICANON | ISIG;

#ifdef ECHONL
	lflag |= ECHONL;
#endif
#ifdef IEXTEN
	lflag |= IEXTEN;
#endif

	mode->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	/* Keep output processing enabled so prompt output such as "\n" remains usable. */
	mode->c_lflag &= ~lflag;
	mode->c_cflag &= ~(CSIZE | PARENB);
	mode->c_cflag |= CS8;
	mode->c_cc[VMIN] = 1;
	mode->c_cc[VTIME] = 0;
}

static bool terminal_enable_stream_raw_mode(terminal_native_stream fd, bool marks_standard_input, terminal_saved_mode *saved)
{
	struct termios mode;
	struct termios raw_mode;

	(void) marks_standard_input;

	if (!terminal_native_stream_is_valid(fd) || tcgetattr(fd, &mode) != 0) {
		return false;
	}

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);

	if (tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return false;
	}

	memset(saved, 0, sizeof(*saved));
	memcpy(saved->magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = fd;
	saved->mode = mode;

	return true;
}

static bool terminal_restore_stream_mode(const terminal_saved_mode *saved)
{
	return saved->stream >= 0 && tcsetattr(saved->stream, TCSANOW, &saved->mode) == 0;
}

static int64_t terminal_current_time_ms(void)
{
#ifdef CLOCK_MONOTONIC
	struct timespec monotonic_now;

	if (clock_gettime(CLOCK_MONOTONIC, &monotonic_now) == 0) {
		return ((int64_t) monotonic_now.tv_sec * 1000) + (monotonic_now.tv_nsec / 1000000);
	}
#endif

	struct timeval now;

	if (gettimeofday(&now, NULL) != 0) {
		return 0;
	}

	return ((int64_t) now.tv_sec * 1000) + (now.tv_usec / 1000);
}

static int terminal_timeout_to_ms(double timeout, bool timeout_is_null)
{
	double timeout_ms;
	int timeout_ms_int;

	if (timeout_is_null) {
		return -1;
	}

	if (timeout <= 0) {
		return 0;
	}

	timeout_ms = timeout * 1000;
	if (timeout_ms >= INT_MAX) {
		return INT_MAX;
	}

	if (timeout_ms < 1) {
		return 1;
	}

	timeout_ms_int = (int) timeout_ms;

	return timeout_ms_int < timeout_ms ? timeout_ms_int + 1 : timeout_ms_int;
}

static int terminal_wait_for_input(int fd, int timeout_ms)
{
	fd_set readfds;
	struct timeval timeout;
	struct timeval *timeout_ptr = NULL;

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);

	if (timeout_ms >= 0) {
		timeout.tv_sec = timeout_ms / 1000;
		timeout.tv_usec = (timeout_ms % 1000) * 1000;
		timeout_ptr = &timeout;
	}

	return select(fd + 1, &readfds, NULL, NULL, timeout_ptr);
}

static int terminal_read_byte(int fd, unsigned char *byte, int timeout_ms, bool report_resize)
{
	int64_t deadline_ms = timeout_ms > 0 ? terminal_current_time_ms() + timeout_ms : 0;

	for (;;) {
		int wait_ms = timeout_ms;
		int ready;
		ssize_t bytes_read;

		if (timeout_ms > 0) {
			int64_t remaining_ms = deadline_ms - terminal_current_time_ms();

			if (remaining_ms <= 0) {
				return 0;
			}

			wait_ms = remaining_ms > INT_MAX ? INT_MAX : (int) remaining_ms;
		}

		ready = terminal_wait_for_input(fd, wait_ms);
		if (ready == 0) {
			return 0;
		}

		if (ready < 0) {
			if (errno == EINTR) {
				if (terminal_consume_resize_pending()) {
					if (report_resize) {
						return TERMINAL_READ_RESIZE;
					}

					continue;
				}

				continue;
			}

			return -1;
		}

		bytes_read = read(fd, byte, 1);
		if (bytes_read == 1) {
			return 1;
		}

		if (bytes_read < 0) {
			if (errno == EINTR) {
				if (terminal_consume_resize_pending()) {
					if (report_resize) {
						return TERMINAL_READ_RESIZE;
					}

					continue;
				}

				continue;
			}

#ifdef EAGAIN
			if (errno == EAGAIN) {
				continue;
			}
#endif
#if defined(EWOULDBLOCK) && (!defined(EAGAIN) || EWOULDBLOCK != EAGAIN)
			if (errno == EWOULDBLOCK) {
				continue;
			}
#endif

			return -1;
		}

		return 0;
	}
}

static zend_string *terminal_key_from_csi_sequence(const unsigned char *sequence, size_t sequence_len)
{
	unsigned char final;

	if (sequence_len == 0) {
		return terminal_key_string("escape");
	}

	final = sequence[sequence_len - 1];

	switch (final) {
		case 'A':
			if (sequence_len == 1) {
				return terminal_key_string("up");
			}
			break;
		case 'B':
			if (sequence_len == 1) {
				return terminal_key_string("down");
			}
			break;
		case 'C':
			if (sequence_len == 1) {
				return terminal_key_string("right");
			}
			break;
		case 'D':
			if (sequence_len == 1) {
				return terminal_key_string("left");
			}
			break;
		case 'H':
			if (sequence_len == 1) {
				return terminal_key_string("home");
			}
			break;
		case 'F':
			if (sequence_len == 1) {
				return terminal_key_string("end");
			}
			break;
		case '~':
		{
			size_t i = 0;
			unsigned int number = 0;

			while (i < sequence_len - 1 && sequence[i] >= '0' && sequence[i] <= '9') {
				number = (number * 10) + (unsigned int) (sequence[i] - '0');
				i++;
			}

			if (i == 0 || (i < sequence_len - 1 && sequence[i] != ';')) {
				break;
			}

			switch (number) {
				case 1:
				case 7:
					return terminal_key_string("home");
				case 3:
					return terminal_key_string("delete");
				case 4:
				case 8:
					return terminal_key_string("end");
				case 5:
					return terminal_key_string("pageup");
				case 6:
					return terminal_key_string("pagedown");
				case 11:
					return terminal_key_string("f1");
				case 12:
					return terminal_key_string("f2");
				case 13:
					return terminal_key_string("f3");
				case 14:
					return terminal_key_string("f4");
				case 15:
					return terminal_key_string("f5");
				case 17:
					return terminal_key_string("f6");
				case 18:
					return terminal_key_string("f7");
				case 19:
					return terminal_key_string("f8");
				case 20:
					return terminal_key_string("f9");
				case 21:
					return terminal_key_string("f10");
				case 23:
					return terminal_key_string("f11");
				case 24:
					return terminal_key_string("f12");
			}
		}
	}

	return terminal_key_string("escape");
}

static zend_string *terminal_key_from_ss3_sequence(unsigned char key)
{
	switch (key) {
		case 'A':
			return terminal_key_string("up");
		case 'B':
			return terminal_key_string("down");
		case 'C':
			return terminal_key_string("right");
		case 'D':
			return terminal_key_string("left");
		case 'H':
			return terminal_key_string("home");
		case 'F':
			return terminal_key_string("end");
		case 'P':
			return terminal_key_string("f1");
		case 'Q':
			return terminal_key_string("f2");
		case 'R':
			return terminal_key_string("f3");
		case 'S':
			return terminal_key_string("f4");
	}

	return terminal_key_string("escape");
}

static bool terminal_is_csi_final_byte(unsigned char key)
{
	return (key >= 'A' && key <= 'Z') || key == '~';
}

static zend_string *terminal_key_from_escape_sequence(int fd, int sequence_timeout_ms)
{
	unsigned char sequence[24];
	size_t sequence_len = 0;
	int result = terminal_read_byte(fd, &sequence[sequence_len++], sequence_timeout_ms, false);

	if (result != 1) {
		return terminal_key_string("escape");
	}

	if (sequence[0] == 'O') {
		result = terminal_read_byte(fd, &sequence[sequence_len++], sequence_timeout_ms, false);
		if (result != 1) {
			return terminal_key_string("escape");
		}

		return terminal_key_from_ss3_sequence(sequence[1]);
	}

	if (sequence[0] != '[') {
		return terminal_key_string("escape");
	}

	while (sequence_len < sizeof(sequence)) {
		result = terminal_read_byte(fd, &sequence[sequence_len++], sequence_timeout_ms, false);
		if (result != 1) {
			return terminal_key_string("escape");
		}

		if (terminal_is_csi_final_byte(sequence[sequence_len - 1])) {
			return terminal_key_from_csi_sequence(sequence + 1, sequence_len - 1);
		}
	}

	return terminal_key_string("escape");
}

static int terminal_sequence_timeout_to_ms(double timeout, bool timeout_is_null)
{
	if (timeout_is_null) {
		return TERMINAL_SEQUENCE_TIMEOUT_MS;
	}

	return terminal_timeout_to_ms(timeout, false);
}

static size_t terminal_utf8_sequence_len(unsigned char key)
{
	if (key >= 0xc2 && key <= 0xdf) {
		return 2;
	}

	if (key >= 0xe0 && key <= 0xef) {
		return 3;
	}

	if (key >= 0xf0 && key <= 0xf4) {
		return 4;
	}

	return 1;
}

static zend_string *terminal_key_from_utf8_sequence(int fd, unsigned char key, int sequence_timeout_ms)
{
	unsigned char sequence[4] = { key };
	size_t sequence_len = terminal_utf8_sequence_len(key);
	size_t i;

	if (sequence_len == 1) {
		return terminal_key_char(key);
	}

	for (i = 1; i < sequence_len; i++) {
		int result = terminal_read_byte(fd, &sequence[i], sequence_timeout_ms, false);

		if (result != 1) {
			return zend_string_init((const char *) sequence, i, false);
		}

		if ((sequence[i] & 0xc0) != 0x80) {
			return zend_string_init((const char *) sequence, i + 1, false);
		}
	}

	return zend_string_init((const char *) sequence, sequence_len, false);
}

static void terminal_secret_append_utf8_sequence(int fd, smart_str *secret, unsigned char key)
{
	unsigned char sequence[4] = { key };
	size_t sequence_len = terminal_utf8_sequence_len(key);
	size_t i;

	if (sequence_len == 1) {
		smart_str_appendc(secret, (char) key);
		return;
	}

	for (i = 1; i < sequence_len; i++) {
		int result = terminal_read_byte(fd, &sequence[i], TERMINAL_SEQUENCE_TIMEOUT_MS, false);

		if (result != 1) {
			smart_str_appendl(secret, (const char *) sequence, i);
			return;
		}

		if ((sequence[i] & 0xc0) != 0x80) {
			smart_str_appendl(secret, (const char *) sequence, i + 1);
			return;
		}
	}

	smart_str_appendl(secret, (const char *) sequence, sequence_len);
}

static zend_string *terminal_key_from_byte(int fd, unsigned char key, int sequence_timeout_ms)
{
	switch (key) {
		case '\r':
		case '\n':
			return terminal_key_string("enter");
		case '\t':
			return terminal_key_string("tab");
		case 0x7f:
		case '\b':
			return terminal_key_string("backspace");
		case 0x1b:
			return terminal_key_from_escape_sequence(fd, sequence_timeout_ms);
		default:
			return terminal_key_from_utf8_sequence(fd, key, sequence_timeout_ms);
	}
}

static zend_string *terminal_read_stdin_key(double timeout, bool timeout_is_null, double sequence_timeout, bool sequence_timeout_is_null)
{
	int fd = terminal_native_stream_from_id(TERMINAL_STREAM_STDIN);
	int timeout_ms = terminal_timeout_to_ms(timeout, timeout_is_null);
	int sequence_timeout_ms = terminal_sequence_timeout_to_ms(sequence_timeout, sequence_timeout_is_null);
	struct termios mode;
	struct termios raw_mode;
	unsigned char key;
	int read_result;
	zend_string *result = NULL;
	bool mode_changed;
#ifdef SIGWINCH
	struct sigaction old_resize_action;
#endif
	bool resize_handler_installed;

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	resize_handler_installed = terminal_install_resize_handler(
#ifdef SIGWINCH
		&old_resize_action
#else
		NULL
#endif
	);

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);
	mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		if (resize_handler_installed) {
			terminal_restore_resize_handler(
#ifdef SIGWINCH
				&old_resize_action
#else
				NULL
#endif
			);
		}

		return NULL;
	}

	read_result = terminal_read_byte(fd, &key, timeout_ms, true);
	if (read_result == TERMINAL_READ_RESIZE) {
		result = terminal_key_string("resize");
	} else if (read_result == 1) {
		result = terminal_key_from_byte(fd, key, sequence_timeout_ms);
	}

	if (mode_changed && tcsetattr(fd, TCSANOW, &mode) != 0) {
		if (resize_handler_installed) {
			terminal_restore_resize_handler(
#ifdef SIGWINCH
				&old_resize_action
#else
				NULL
#endif
			);
		}

		if (result != NULL) {
			zend_string_release(result);
		}

		return NULL;
	}

	if (resize_handler_installed) {
		terminal_restore_resize_handler(
#ifdef SIGWINCH
			&old_resize_action
#else
			NULL
#endif
		);
	}

	return result;
}

static zend_string *terminal_read_stdin_secret(void)
{
	int fd = terminal_native_stream_from_id(TERMINAL_STREAM_STDIN);
	struct termios mode;
	struct termios raw_mode;
	smart_str secret = {0};
	bool success = false;
	bool mode_changed;

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);
	mode_changed = memcmp(&raw_mode, &mode, sizeof(mode)) != 0;

	if (mode_changed && tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return NULL;
	}

	for (;;) {
		unsigned char key;
		int result;

		result = terminal_read_byte(fd, &key, -1, false);
		if (result != 1) {
			break;
		}

		switch (key) {
			case '\r':
			case '\n':
				success = true;
				goto restore;
			case 0x7f:
			case '\b':
				if (terminal_buffer_remove_last_utf8_char(&secret)) {
					zend_long written;

					terminal_stream_write(terminal_native_stream_from_id(TERMINAL_STREAM_STDOUT), "\b \b", sizeof("\b \b") - 1, &written);
				}
				break;
			case 0x03:
			case 0x04:
				goto restore;
			case 0x1b:
			{
				zend_string *escape_key = terminal_key_from_escape_sequence(fd, TERMINAL_SEQUENCE_TIMEOUT_MS);
				bool is_escape = zend_string_equals_literal(escape_key, "escape");

				zend_string_release(escape_key);

				if (is_escape) {
					goto restore;
				}

				break;
			}
			default:
				if (key >= 0x20) {
					size_t before_len = secret.s == NULL ? 0 : ZSTR_LEN(secret.s);
					zend_long written;

					terminal_secret_append_utf8_sequence(fd, &secret, key);
					if ((secret.s == NULL ? 0 : ZSTR_LEN(secret.s)) > before_len) {
						terminal_stream_write(terminal_native_stream_from_id(TERMINAL_STREAM_STDOUT), "*", 1, &written);
					}
				}
				break;
		}
	}

restore:
	if (mode_changed && tcsetattr(fd, TCSANOW, &mode) != 0) {
		smart_str_free(&secret);
		return NULL;
	}

	if (!success) {
		smart_str_free(&secret);
		return NULL;
	}

	if (secret.s == NULL) {
		return zend_string_init("", 0, false);
	}

	smart_str_0(&secret);

	return secret.s;
}
#endif

typedef struct _terminal_stream_target {
	bool is_enum;
	zend_long enum_id;
	php_stream *php_stream;
	zval *stream_resource;
	terminal_native_stream native_stream;
} terminal_stream_target;

static bool terminal_stream_target_init(
	zval *stream_arg,
	zend_long default_stream,
	uint32_t arg_num,
	terminal_stream_target *target
)
{
	memset(target, 0, sizeof(*target));
	target->is_enum = true;
	target->enum_id = default_stream;
	target->native_stream = terminal_native_stream_from_id(default_stream);

	if (stream_arg == NULL) {
		return true;
	}

	if (Z_TYPE_P(stream_arg) == IS_OBJECT
		&& instanceof_function(Z_OBJCE_P(stream_arg), terminal_stream_ce)) {
		target->enum_id = terminal_stream_from_enum(stream_arg);
		target->native_stream = terminal_native_stream_from_id(target->enum_id);
		return true;
	}

	if (Z_TYPE_P(stream_arg) == IS_RESOURCE) {
		target->php_stream = (php_stream *) zend_fetch_resource2(
			Z_RES_P(stream_arg),
			"stream",
			php_file_le_stream(),
			php_file_le_pstream()
		);
		if (target->php_stream == NULL) {
			return false;
		}

		target->is_enum = false;
		target->stream_resource = stream_arg;
		target->native_stream = terminal_native_stream_from_php_stream(target->php_stream);
		return true;
	}

	zend_argument_type_error(
		arg_num,
		"must be of type Terminal\\Stream|resource, %s given",
		zend_zval_type_name(stream_arg)
	);

	return false;
}

static bool terminal_php_stream_write_all(php_stream *stream, const char *buffer, size_t buffer_len, zend_long *written)
{
	size_t total = 0;

	while (total < buffer_len) {
		ssize_t chunk = php_stream_write(stream, buffer + total, buffer_len - total);

		if (chunk <= 0) {
			return false;
		}

		total += (size_t) chunk;
	}

	*written = (zend_long) total;

	return true;
}

static bool terminal_mode_token_stream_is_valid(const terminal_mode_token_object *mode)
{
	php_stream *stream;

	if (Z_ISUNDEF(mode->stream_resource)) {
		return true;
	}

	if (Z_TYPE(mode->stream_resource) != IS_RESOURCE) {
		return false;
	}

	stream = (php_stream *) zend_fetch_resource2(
		Z_RES(mode->stream_resource),
		NULL,
		php_file_le_stream(),
		php_file_le_pstream()
	);

	return stream != NULL
		&& terminal_native_stream_from_php_stream(stream) == mode->saved.stream;
}

static void terminal_validate_output_stream_or_throw(zend_long stream, uint32_t arg_num)
{
	if (stream != TERMINAL_STREAM_STDOUT && stream != TERMINAL_STREAM_STDERR) {
		zend_argument_value_error(arg_num, "must be Terminal\\Stream::Stdout or Terminal\\Stream::Stderr");
	}
}

static void terminal_validate_input_stream_or_throw(zend_long stream, uint32_t arg_num)
{
	if (stream != TERMINAL_STREAM_STDIN) {
		zend_argument_value_error(arg_num, "must be Terminal\\Stream::Stdin");
	}
}

ZEND_METHOD(Terminal_ModeToken, __construct)
{
	(void) return_value;

	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(Terminal_Terminal, getBackend)
{
	ZEND_PARSE_PARAMETERS_NONE();

#ifdef PHP_WIN32
	terminal_set_enum_case(return_value, terminal_backend_ce, "Windows");
#else
	terminal_set_enum_case(return_value, terminal_backend_ce, "Posix");
#endif
}

ZEND_METHOD(Terminal_Terminal, isTty)
{
	zval *stream_arg = NULL;
	terminal_stream_target stream;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(stream_arg)
	ZEND_PARSE_PARAMETERS_END();

	if (!terminal_stream_target_init(stream_arg, TERMINAL_STREAM_STDOUT, 1, &stream)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_stream_is_tty(stream.native_stream));
}

ZEND_METHOD(Terminal_Terminal, supportsAnsi)
{
	zval *stream_arg = NULL;
	terminal_stream_target stream;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(stream_arg)
	ZEND_PARSE_PARAMETERS_END();

	if (!terminal_stream_target_init(stream_arg, TERMINAL_STREAM_STDOUT, 1, &stream)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_stream_supports_ansi(stream.native_stream));
}

ZEND_METHOD(Terminal_Terminal, enableAnsi)
{
	zval *stream_arg = NULL;
	terminal_stream_target stream;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(stream_arg)
	ZEND_PARSE_PARAMETERS_END();

	if (!terminal_stream_target_init(stream_arg, TERMINAL_STREAM_STDOUT, 1, &stream)) {
		RETURN_THROWS();
	}

	if (stream.is_enum) {
		terminal_validate_output_stream_or_throw(stream.enum_id, 1);
	}
	if (EG(exception)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_enable_stream_ansi(stream.native_stream));
}

ZEND_METHOD(Terminal_Terminal, getSize)
{
	zval *stream_arg = NULL;
	terminal_stream_target stream;
	zend_long columns;
	zend_long rows;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(stream_arg)
	ZEND_PARSE_PARAMETERS_END();

	if (!terminal_stream_target_init(stream_arg, TERMINAL_STREAM_STDOUT, 1, &stream)) {
		RETURN_THROWS();
	}

	if (!terminal_stream_size(stream.native_stream, &columns, &rows)
		&& !terminal_size_from_environment(&columns, &rows)) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "cols", columns);
	add_assoc_long(return_value, "rows", rows);
}

ZEND_METHOD(Terminal_Terminal, write)
{
	zend_string *data;
	zval *stream_arg = NULL;
	terminal_stream_target stream;
	zend_long written;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(data)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(stream_arg)
	ZEND_PARSE_PARAMETERS_END();

	if (!terminal_stream_target_init(stream_arg, TERMINAL_STREAM_STDOUT, 2, &stream)) {
		RETURN_THROWS();
	}

	if (stream.is_enum) {
		terminal_validate_output_stream_or_throw(stream.enum_id, 2);
	}
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (stream.php_stream != NULL) {
		if (!terminal_php_stream_write_all(stream.php_stream, ZSTR_VAL(data), ZSTR_LEN(data), &written)) {
			RETURN_FALSE;
		}
	} else if (!terminal_stream_write(stream.native_stream, ZSTR_VAL(data), ZSTR_LEN(data), &written)) {
		RETURN_FALSE;
	}

	RETURN_LONG(written);
}

ZEND_METHOD(Terminal_Terminal, enableRawMode)
{
	zval *stream_arg = NULL;
	terminal_stream_target stream;
	terminal_saved_mode saved;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(stream_arg)
	ZEND_PARSE_PARAMETERS_END();

	if (!terminal_stream_target_init(stream_arg, TERMINAL_STREAM_STDIN, 1, &stream)) {
		RETURN_THROWS();
	}

	if (stream.is_enum) {
		terminal_validate_input_stream_or_throw(stream.enum_id, 1);
	}
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!terminal_enable_stream_raw_mode(
		stream.native_stream,
		stream.native_stream == terminal_native_stream_from_id(TERMINAL_STREAM_STDIN),
		&saved
	)) {
		RETURN_FALSE;
	}

	terminal_create_mode_token(return_value, &saved, stream.stream_resource);
}

ZEND_METHOD(Terminal_Terminal, restoreMode)
{
	zval *mode_token;
	terminal_mode_token_object *mode;
	bool restored;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(mode_token, terminal_mode_token_ce)
	ZEND_PARSE_PARAMETERS_END();

	mode = Z_TERMINAL_MODE_TOKEN_P(mode_token);

	if (!mode->valid || memcmp(mode->saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) != 0) {
		zend_argument_value_error(1, "must be a terminal mode token returned by Terminal\\Terminal::enableRawMode()");
		RETURN_THROWS();
	}

	if (!terminal_mode_token_stream_is_valid(mode)) {
		RETURN_FALSE;
	}

	restored = terminal_restore_stream_mode(&mode->saved);
	if (restored) {
		if (!Z_ISUNDEF(mode->stream_resource)) {
			zval_ptr_dtor(&mode->stream_resource);
			ZVAL_UNDEF(&mode->stream_resource);
		}
		memset(&mode->saved, 0, sizeof(mode->saved));
		mode->valid = false;
	}

	RETURN_BOOL(restored);
}

ZEND_METHOD(Terminal_Terminal, readKey)
{
	double timeout = 0;
	double sequence_timeout = 0;
	bool timeout_is_null = true;
	bool sequence_timeout_is_null = true;
	zend_string *key;
	zend_object *key_case;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE_OR_NULL(timeout, timeout_is_null)
		Z_PARAM_DOUBLE_OR_NULL(sequence_timeout, sequence_timeout_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (!timeout_is_null && timeout < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (!sequence_timeout_is_null && sequence_timeout < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	key = terminal_read_stdin_key(timeout, timeout_is_null, sequence_timeout, sequence_timeout_is_null);
	if (key == NULL) {
		RETURN_FALSE;
	}

	key_case = terminal_key_enum_from_string(key);
	if (key_case != NULL) {
		zend_string_release(key);
		RETURN_OBJ_COPY(key_case);
	}

	RETURN_STR(key);
}

ZEND_METHOD(Terminal_Terminal, readSecret)
{
	zend_string *prompt = NULL;
	zend_string *secret;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(prompt)
	ZEND_PARSE_PARAMETERS_END();

	if (prompt != NULL && ZSTR_LEN(prompt) > 0) {
		zend_long written;

		if (!terminal_stream_write(terminal_native_stream_from_id(TERMINAL_STREAM_STDOUT), ZSTR_VAL(prompt), ZSTR_LEN(prompt), &written)
			|| written != (zend_long) ZSTR_LEN(prompt)) {
			zend_throw_error(NULL, "Unable to write secret prompt");
			RETURN_THROWS();
		}
	}

	secret = terminal_read_stdin_secret();
	if (secret != NULL) {
		RETURN_STR(secret);
	}

	zend_throw_error(NULL, "Unable to read secret from terminal");
	RETURN_THROWS();
}

PHP_MINIT_FUNCTION(terminal)
{
	(void) type;
	(void) module_number;

#if defined(ZTS) && defined(COMPILE_DL_TERMINAL)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	terminal_backend_ce = register_class_Terminal_Backend();
	terminal_stream_ce = register_class_Terminal_Stream();
	terminal_key_ce = register_class_Terminal_Key();
	terminal_mode_token_ce = register_class_Terminal_ModeToken();
	terminal_mode_token_ce->create_object = terminal_mode_token_create_object;
	terminal_mode_token_ce->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	memcpy(&terminal_mode_token_handlers, zend_get_std_object_handlers(), sizeof(terminal_mode_token_handlers));
	terminal_mode_token_handlers.offset = offsetof(terminal_mode_token_object, std);
	terminal_mode_token_handlers.free_obj = terminal_mode_token_free_obj;
	terminal_mode_token_handlers.clone_obj = NULL;

	register_class_Terminal_Terminal();

	return SUCCESS;
}

PHP_MINFO_FUNCTION(terminal)
{
	(void) zend_module;

	php_info_print_table_start();
	php_info_print_table_row(2, "terminal support", "enabled");
#ifdef PHP_WIN32
	php_info_print_table_row(2, "backend", "windows");
#else
	php_info_print_table_row(2, "backend", "posix");
#endif
	php_info_print_table_end();
}

zend_module_entry terminal_module_entry = {
	STANDARD_MODULE_HEADER,
	"terminal",
	NULL,
	PHP_MINIT(terminal),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(terminal),
	PHP_TERMINAL_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TERMINAL
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(terminal)
#endif
