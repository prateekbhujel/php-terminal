/* terminal extension for PHP (c) 2026 Pratik Bhujel */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_enum.h"
#include "ext/standard/info.h"
#include "php_terminal.h"
#include "terminal_arginfo.h"

#ifdef PHP_WIN32
# include <windows.h>
#else
# include <errno.h>
# include <limits.h>
# include <stdint.h>
# include <sys/select.h>
# include <sys/time.h>
# include <termios.h>
# include <unistd.h>
# include <sys/ioctl.h>
#endif

#define TERMINAL_MODE_TOKEN_MAGIC "PHPTTY1"
#define TERMINAL_MODE_TOKEN_MAGIC_LEN (sizeof(TERMINAL_MODE_TOKEN_MAGIC) - 1)
#define TERMINAL_ESCAPE_TIMEOUT_MS 25

typedef struct _terminal_saved_mode {
	char magic[TERMINAL_MODE_TOKEN_MAGIC_LEN];
	zend_long stream;
#ifdef PHP_WIN32
	DWORD mode;
#else
	struct termios mode;
#endif
} terminal_saved_mode;

static zend_class_entry *terminal_backend_ce;
static zend_class_entry *terminal_stream_ce;
static zend_class_entry *terminal_key_ce;

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

	return NULL;
}

static void terminal_buffer_remove_last_utf8_char(smart_str *buffer)
{
	size_t len;

	if (buffer->s == NULL || ZSTR_LEN(buffer->s) == 0) {
		return;
	}

	len = ZSTR_LEN(buffer->s);

	do {
		len--;
	} while (len > 0 && (((unsigned char) ZSTR_VAL(buffer->s)[len]) & 0xc0) == 0x80);

	ZSTR_VAL(buffer->s)[len] = '\0';
	ZSTR_LEN(buffer->s) = len;
}

#ifdef PHP_WIN32
static HANDLE terminal_handle_from_id(zend_long stream)
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

static bool terminal_stream_is_tty(zend_long stream)
{
	HANDLE handle = terminal_handle_from_id(stream);
	DWORD mode;

	return handle != INVALID_HANDLE_VALUE
		&& handle != NULL
		&& GetConsoleMode(handle, &mode);
}

static bool terminal_stream_supports_ansi(zend_long stream)
{
	HANDLE handle = terminal_handle_from_id(stream);
	DWORD mode;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
		return false;
	}

	if (!GetConsoleMode(handle, &mode)) {
		return false;
	}

	return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}

static bool terminal_enable_stream_ansi(zend_long stream)
{
	HANDLE handle = terminal_handle_from_id(stream);
	DWORD mode;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
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

static bool terminal_stream_size(zend_long stream, zend_long *columns, zend_long *rows)
{
	HANDLE handle = terminal_handle_from_id(stream);
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
		return false;
	}

	if (!GetConsoleScreenBufferInfo(handle, &info)) {
		return false;
	}

	*columns = (zend_long) (info.srWindow.Right - info.srWindow.Left + 1);
	*rows = (zend_long) (info.srWindow.Bottom - info.srWindow.Top + 1);

	return true;
}

static bool terminal_stream_write(zend_long stream, const char *buffer, size_t buffer_len, zend_long *written)
{
	HANDLE handle = terminal_handle_from_id(stream);
	size_t total = 0;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
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

static DWORD terminal_make_raw_mode(DWORD mode)
{
	/* ReadConsoleInputW returns key events directly, so VT input is not required here. */
	return mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
}

static bool terminal_enable_stream_raw_mode(zend_long stream, terminal_saved_mode *saved)
{
	HANDLE handle = terminal_handle_from_id(stream);
	DWORD mode;
	DWORD raw_mode;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL || !GetConsoleMode(handle, &mode)) {
		return false;
	}

	raw_mode = terminal_make_raw_mode(mode);

	if (!SetConsoleMode(handle, raw_mode)) {
		return false;
	}

	memset(saved, 0, sizeof(*saved));
	memcpy(saved->magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = stream;
	saved->mode = mode;

	return true;
}

static bool terminal_restore_stream_mode(const terminal_saved_mode *saved)
{
	HANDLE handle = terminal_handle_from_id(saved->stream);

	return handle != INVALID_HANDLE_VALUE
		&& handle != NULL
		&& SetConsoleMode(handle, saved->mode);
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

static zend_string *terminal_read_stdin_key(double timeout, bool timeout_is_null)
{
	HANDLE handle = terminal_handle_from_id(TERMINAL_STREAM_STDIN);
	DWORD mode;
	DWORD raw_mode;
	DWORD wait_ms = terminal_timeout_to_wait_ms(timeout, timeout_is_null);
	ULONGLONG deadline_ms = wait_ms == INFINITE ? 0 : GetTickCount64() + wait_ms;
	zend_string *result = NULL;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL || !GetConsoleMode(handle, &mode)) {
		return NULL;
	}

	raw_mode = terminal_make_raw_mode(mode);
	if (!SetConsoleMode(handle, raw_mode)) {
		return NULL;
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

	if (!SetConsoleMode(handle, mode)) {
		if (result != NULL) {
			zend_string_release(result);
		}

		return NULL;
	}

	return result;
}

static zend_string *terminal_read_stdin_secret(double timeout, bool timeout_is_null)
{
	HANDLE handle = terminal_handle_from_id(TERMINAL_STREAM_STDIN);
	DWORD mode;
	DWORD raw_mode;
	DWORD wait_ms = terminal_timeout_to_wait_ms(timeout, timeout_is_null);
	ULONGLONG deadline_ms = wait_ms == INFINITE ? 0 : GetTickCount64() + wait_ms;
	smart_str secret = {0};
	bool success = false;
	bool failed = false;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL || !GetConsoleMode(handle, &mode)) {
		return NULL;
	}

	raw_mode = terminal_make_raw_mode(mode);
	if (!SetConsoleMode(handle, raw_mode)) {
		return NULL;
	}

	for (;;) {
		INPUT_RECORD record;
		KEY_EVENT_RECORD *key;
		DWORD records_read;
		DWORD wait_result = WaitForSingleObject(handle, wait_ms);

		if (wait_result == WAIT_TIMEOUT) {
			break;
		}

		if (wait_result != WAIT_OBJECT_0) {
			failed = true;
			break;
		}

		if (!ReadConsoleInputW(handle, &record, 1, &records_read) || records_read != 1) {
			failed = true;
			break;
		}

		if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown) {
			goto update_timeout;
		}

		key = &record.Event.KeyEvent;

		if (key->wVirtualKeyCode == VK_RETURN) {
			success = true;
			break;
		}

		if (key->wVirtualKeyCode == VK_BACK) {
			terminal_buffer_remove_last_utf8_char(&secret);
			goto update_timeout;
		}

		if (key->wVirtualKeyCode == VK_ESCAPE || key->uChar.UnicodeChar == 0x03 || key->uChar.UnicodeChar == 0x04) {
			break;
		}

		if (!terminal_secret_append_wchar(&secret, key->uChar.UnicodeChar)) {
			failed = true;
			break;
		}

update_timeout:
		if (!timeout_is_null) {
			wait_ms = terminal_remaining_wait_ms(deadline_ms);
			if (wait_ms == 0) {
				break;
			}
		}
	}

	if (!SetConsoleMode(handle, mode)) {
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
static int terminal_fd_from_id(zend_long stream)
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

static bool terminal_stream_is_tty(zend_long stream)
{
	int fd = terminal_fd_from_id(stream);

	return fd >= 0 && isatty(fd) == 1;
}

static bool terminal_stream_supports_ansi(zend_long stream)
{
	const char *term;

	if (!terminal_stream_is_tty(stream)) {
		return false;
	}

	term = getenv("TERM");
	if (term == NULL || *term == '\0' || strcmp(term, "dumb") == 0) {
		return false;
	}

	return true;
}

static bool terminal_enable_stream_ansi(zend_long stream)
{
	return terminal_stream_supports_ansi(stream);
}

static bool terminal_stream_size(zend_long stream, zend_long *columns, zend_long *rows)
{
	int fd = terminal_fd_from_id(stream);
	struct winsize ws;

	if (fd < 0) {
		return false;
	}

	if (ioctl(fd, TIOCGWINSZ, &ws) != 0 || ws.ws_col == 0 || ws.ws_row == 0) {
		return false;
	}

	*columns = (zend_long) ws.ws_col;
	*rows = (zend_long) ws.ws_row;

	return true;
}

static bool terminal_stream_write(zend_long stream, const char *buffer, size_t buffer_len, zend_long *written)
{
	int fd = terminal_fd_from_id(stream);
	size_t total = 0;

	if (fd < 0) {
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

static bool terminal_enable_stream_raw_mode(zend_long stream, terminal_saved_mode *saved)
{
	int fd = terminal_fd_from_id(stream);
	struct termios mode;
	struct termios raw_mode;

	if (fd < 0 || tcgetattr(fd, &mode) != 0) {
		return false;
	}

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);

	if (tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return false;
	}

	memset(saved, 0, sizeof(*saved));
	memcpy(saved->magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN);
	saved->stream = stream;
	saved->mode = mode;

	return true;
}

static bool terminal_restore_stream_mode(const terminal_saved_mode *saved)
{
	int fd = terminal_fd_from_id(saved->stream);

	return fd >= 0 && tcsetattr(fd, TCSANOW, &saved->mode) == 0;
}

static int64_t terminal_current_time_ms(void)
{
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

static int terminal_read_byte(int fd, unsigned char *byte, int timeout_ms)
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
	unsigned char final = sequence[sequence_len - 1];

	switch (final) {
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
		case '~':
			switch (sequence[0]) {
				case '1':
				case '7':
					return terminal_key_string("home");
				case '3':
					return terminal_key_string("delete");
				case '4':
				case '8':
					return terminal_key_string("end");
				case '5':
					return terminal_key_string("pageup");
				case '6':
					return terminal_key_string("pagedown");
			}
	}

	return terminal_key_string("escape");
}

static zend_string *terminal_key_from_escape_sequence(int fd)
{
	unsigned char sequence[8];
	size_t sequence_len = 0;
	int result = terminal_read_byte(fd, &sequence[sequence_len++], TERMINAL_ESCAPE_TIMEOUT_MS);

	if (result <= 0) {
		return terminal_key_string("escape");
	}

	if (sequence[0] == 'O') {
		result = terminal_read_byte(fd, &sequence[sequence_len++], TERMINAL_ESCAPE_TIMEOUT_MS);
		if (result <= 0) {
			return terminal_key_string("escape");
		}

		return terminal_key_from_csi_sequence(sequence + 1, 1);
	}

	if (sequence[0] != '[') {
		return terminal_key_string("escape");
	}

	do {
		result = terminal_read_byte(fd, &sequence[sequence_len++], TERMINAL_ESCAPE_TIMEOUT_MS);
		if (result <= 0) {
			return terminal_key_string("escape");
		}
	} while (sequence_len < sizeof(sequence) && !((sequence[sequence_len - 1] >= 'A' && sequence[sequence_len - 1] <= 'Z') || sequence[sequence_len - 1] == '~'));

	return terminal_key_from_csi_sequence(sequence + 1, sequence_len - 1);
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

static zend_string *terminal_key_from_utf8_sequence(int fd, unsigned char key)
{
	unsigned char sequence[4] = { key };
	size_t sequence_len = terminal_utf8_sequence_len(key);
	size_t i;

	if (sequence_len == 1) {
		return terminal_key_char(key);
	}

	for (i = 1; i < sequence_len; i++) {
		int result = terminal_read_byte(fd, &sequence[i], TERMINAL_ESCAPE_TIMEOUT_MS);

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
		int result = terminal_read_byte(fd, &sequence[i], TERMINAL_ESCAPE_TIMEOUT_MS);

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

static zend_string *terminal_key_from_byte(int fd, unsigned char key)
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
			return terminal_key_from_escape_sequence(fd);
		default:
			return terminal_key_from_utf8_sequence(fd, key);
	}
}

static zend_string *terminal_read_stdin_key(double timeout, bool timeout_is_null)
{
	int fd = terminal_fd_from_id(TERMINAL_STREAM_STDIN);
	int timeout_ms = terminal_timeout_to_ms(timeout, timeout_is_null);
	struct termios mode;
	struct termios raw_mode;
	unsigned char key;
	zend_string *result = NULL;

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);

	if (tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return NULL;
	}

	if (terminal_read_byte(fd, &key, timeout_ms) == 1) {
		result = terminal_key_from_byte(fd, key);
	}

	if (tcsetattr(fd, TCSANOW, &mode) != 0) {
		if (result != NULL) {
			zend_string_release(result);
		}

		return NULL;
	}

	return result;
}

static zend_string *terminal_read_stdin_secret(double timeout, bool timeout_is_null)
{
	int fd = terminal_fd_from_id(TERMINAL_STREAM_STDIN);
	int timeout_ms = terminal_timeout_to_ms(timeout, timeout_is_null);
	int64_t deadline_ms = timeout_ms > 0 ? terminal_current_time_ms() + timeout_ms : 0;
	struct termios mode;
	struct termios raw_mode;
	smart_str secret = {0};
	bool success = false;

	if (fd < 0 || isatty(fd) != 1 || tcgetattr(fd, &mode) != 0) {
		return NULL;
	}

	raw_mode = mode;
	terminal_make_raw_mode(&raw_mode);

	if (tcsetattr(fd, TCSANOW, &raw_mode) != 0) {
		return NULL;
	}

	for (;;) {
		unsigned char key;
		int wait_ms = timeout_ms;
		int result;

		if (timeout_ms > 0) {
			int64_t remaining_ms = deadline_ms - terminal_current_time_ms();

			if (remaining_ms <= 0) {
				break;
			}

			wait_ms = remaining_ms > INT_MAX ? INT_MAX : (int) remaining_ms;
		}

		result = terminal_read_byte(fd, &key, wait_ms);
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
				terminal_buffer_remove_last_utf8_char(&secret);
				break;
			case 0x03:
			case 0x04:
				goto restore;
			case 0x1b:
			{
				zend_string *escape_key = terminal_key_from_escape_sequence(fd);
				bool is_escape = zend_string_equals_literal(escape_key, "escape");

				zend_string_release(escape_key);

				if (is_escape) {
					goto restore;
				}

				break;
			}
			default:
				if (key >= 0x20) {
					terminal_secret_append_utf8_sequence(fd, &secret, key);
				}
				break;
		}
	}

restore:
	if (tcsetattr(fd, TCSANOW, &mode) != 0) {
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
	zval *stream_case = NULL;
	zend_long stream = TERMINAL_STREAM_STDOUT;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(stream_case, terminal_stream_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (stream_case != NULL) {
		stream = terminal_stream_from_enum(stream_case);
	}

	RETURN_BOOL(terminal_stream_is_tty(stream));
}

ZEND_METHOD(Terminal_Terminal, supportsAnsi)
{
	zval *stream_case = NULL;
	zend_long stream = TERMINAL_STREAM_STDOUT;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(stream_case, terminal_stream_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (stream_case != NULL) {
		stream = terminal_stream_from_enum(stream_case);
	}

	RETURN_BOOL(terminal_stream_supports_ansi(stream));
}

ZEND_METHOD(Terminal_Terminal, enableAnsi)
{
	zval *stream_case = NULL;
	zend_long stream = TERMINAL_STREAM_STDOUT;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(stream_case, terminal_stream_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (stream_case != NULL) {
		stream = terminal_stream_from_enum(stream_case);
	}

	terminal_validate_output_stream_or_throw(stream, 1);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_enable_stream_ansi(stream));
}

ZEND_METHOD(Terminal_Terminal, getSize)
{
	zval *stream_case = NULL;
	zend_long stream = TERMINAL_STREAM_STDOUT;
	zend_long columns;
	zend_long rows;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(stream_case, terminal_stream_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (stream_case != NULL) {
		stream = terminal_stream_from_enum(stream_case);
	}

	if (!terminal_stream_size(stream, &columns, &rows)) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "columns", columns);
	add_assoc_long(return_value, "rows", rows);
}

ZEND_METHOD(Terminal_Terminal, write)
{
	zend_string *data;
	zval *stream_case = NULL;
	zend_long stream = TERMINAL_STREAM_STDOUT;
	zend_long written;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(data)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(stream_case, terminal_stream_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (stream_case != NULL) {
		stream = terminal_stream_from_enum(stream_case);
	}

	terminal_validate_output_stream_or_throw(stream, 2);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!terminal_stream_write(stream, ZSTR_VAL(data), ZSTR_LEN(data), &written)) {
		RETURN_FALSE;
	}

	RETURN_LONG(written);
}

ZEND_METHOD(Terminal_Terminal, enableRawMode)
{
	zval *stream_case = NULL;
	zend_long stream = TERMINAL_STREAM_STDIN;
	terminal_saved_mode saved;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(stream_case, terminal_stream_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (stream_case != NULL) {
		stream = terminal_stream_from_enum(stream_case);
	}

	terminal_validate_input_stream_or_throw(stream, 1);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!terminal_enable_stream_raw_mode(stream, &saved)) {
		RETURN_FALSE;
	}

	RETURN_STRINGL((const char *) &saved, sizeof(saved));
}

ZEND_METHOD(Terminal_Terminal, restoreMode)
{
	zend_string *mode;
	terminal_saved_mode saved;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(mode)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(mode) != sizeof(saved)) {
		zend_argument_value_error(1, "must be a terminal mode token returned by Terminal\\Terminal::enableRawMode()");
		RETURN_THROWS();
	}

	memcpy(&saved, ZSTR_VAL(mode), sizeof(saved));

	if (memcmp(saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) != 0) {
		zend_argument_value_error(1, "must be a terminal mode token returned by Terminal\\Terminal::enableRawMode()");
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_restore_stream_mode(&saved));
}

ZEND_METHOD(Terminal_Terminal, readKey)
{
	double timeout = 0;
	bool timeout_is_null = true;
	zend_string *key;
	zend_object *key_case;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE_OR_NULL(timeout, timeout_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (!timeout_is_null && timeout < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	key = terminal_read_stdin_key(timeout, timeout_is_null);
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
	double timeout = 0;
	bool timeout_is_null = true;
	zend_string *secret;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE_OR_NULL(timeout, timeout_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (!timeout_is_null && timeout < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	secret = terminal_read_stdin_secret(timeout, timeout_is_null);
	if (secret != NULL) {
		RETURN_STR(secret);
	}

	RETURN_FALSE;
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
