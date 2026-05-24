/* terminal extension for PHP (c) 2026 Pratik Bhujel */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_terminal.h"
#include "terminal_arginfo.h"

#ifdef PHP_WIN32
# include <windows.h>
#else
# include <errno.h>
# include <termios.h>
# include <unistd.h>
# include <sys/ioctl.h>
#endif

#define TERMINAL_MODE_TOKEN_MAGIC "PHPTTY1"
#define TERMINAL_MODE_TOKEN_MAGIC_LEN (sizeof(TERMINAL_MODE_TOKEN_MAGIC) - 1)

typedef struct _terminal_saved_mode {
	char magic[TERMINAL_MODE_TOKEN_MAGIC_LEN];
	zend_long stream;
#ifdef PHP_WIN32
	DWORD mode;
#else
	struct termios mode;
#endif
} terminal_saved_mode;

static zend_always_inline bool terminal_valid_stream(zend_long stream)
{
	return stream >= TERMINAL_STREAM_STDIN && stream <= TERMINAL_STREAM_STDERR;
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
	DWORD out = 0;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
		return false;
	}

	if (!WriteFile(handle, buffer, (DWORD) buffer_len, &out, NULL)) {
		return false;
	}

	*written = (zend_long) out;

	return true;
}

static bool terminal_enable_stream_raw_mode(zend_long stream, terminal_saved_mode *saved)
{
	HANDLE handle = terminal_handle_from_id(stream);
	DWORD mode;
	DWORD raw_mode;

	if (handle == INVALID_HANDLE_VALUE || handle == NULL || !GetConsoleMode(handle, &mode)) {
		return false;
	}

	raw_mode = mode;
	raw_mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
	raw_mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;

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
	mode->c_oflag &= ~OPOST;
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
#endif

static void terminal_validate_stream_or_throw(zend_long stream, uint32_t arg_num)
{
	if (!terminal_valid_stream(stream)) {
		zend_argument_value_error(arg_num, "must be TERMINAL_STDIN, TERMINAL_STDOUT, or TERMINAL_STDERR");
	}
}

static void terminal_validate_output_stream_or_throw(zend_long stream, uint32_t arg_num)
{
	if (stream != TERMINAL_STREAM_STDOUT && stream != TERMINAL_STREAM_STDERR) {
		zend_argument_value_error(arg_num, "must be TERMINAL_STDOUT or TERMINAL_STDERR");
	}
}

static void terminal_validate_input_stream_or_throw(zend_long stream, uint32_t arg_num)
{
	if (stream != TERMINAL_STREAM_STDIN) {
		zend_argument_value_error(arg_num, "must be TERMINAL_STDIN");
	}
}

PHP_FUNCTION(terminal_backend)
{
	ZEND_PARSE_PARAMETERS_NONE();

#ifdef PHP_WIN32
	RETURN_STRING("windows");
#else
	RETURN_STRING("posix");
#endif
}

PHP_FUNCTION(terminal_is_tty)
{
	zend_long stream = TERMINAL_STREAM_STDOUT;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(stream)
	ZEND_PARSE_PARAMETERS_END();

	terminal_validate_stream_or_throw(stream, 1);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_stream_is_tty(stream));
}

PHP_FUNCTION(terminal_supports_ansi)
{
	zend_long stream = TERMINAL_STREAM_STDOUT;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(stream)
	ZEND_PARSE_PARAMETERS_END();

	terminal_validate_stream_or_throw(stream, 1);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_stream_supports_ansi(stream));
}

PHP_FUNCTION(terminal_get_size)
{
	zend_long stream = TERMINAL_STREAM_STDOUT;
	zend_long columns;
	zend_long rows;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(stream)
	ZEND_PARSE_PARAMETERS_END();

	terminal_validate_stream_or_throw(stream, 1);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!terminal_stream_size(stream, &columns, &rows)) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "columns", columns);
	add_assoc_long(return_value, "rows", rows);
}

PHP_FUNCTION(terminal_write)
{
	zend_string *data;
	zend_long stream = TERMINAL_STREAM_STDOUT;
	zend_long written;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(data)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(stream)
	ZEND_PARSE_PARAMETERS_END();

	terminal_validate_output_stream_or_throw(stream, 2);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!terminal_stream_write(stream, ZSTR_VAL(data), ZSTR_LEN(data), &written)) {
		RETURN_FALSE;
	}

	RETURN_LONG(written);
}

PHP_FUNCTION(terminal_enable_raw_mode)
{
	zend_long stream = TERMINAL_STREAM_STDIN;
	terminal_saved_mode saved;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(stream)
	ZEND_PARSE_PARAMETERS_END();

	terminal_validate_input_stream_or_throw(stream, 1);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!terminal_enable_stream_raw_mode(stream, &saved)) {
		RETURN_FALSE;
	}

	RETURN_STRINGL((const char *) &saved, sizeof(saved));
}

PHP_FUNCTION(terminal_restore_mode)
{
	zend_string *mode;
	terminal_saved_mode saved;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(mode)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(mode) != sizeof(saved)) {
		zend_argument_value_error(1, "must be a terminal mode token returned by terminal_enable_raw_mode()");
		RETURN_THROWS();
	}

	memcpy(&saved, ZSTR_VAL(mode), sizeof(saved));

	if (memcmp(saved.magic, TERMINAL_MODE_TOKEN_MAGIC, TERMINAL_MODE_TOKEN_MAGIC_LEN) != 0) {
		zend_argument_value_error(1, "must be a terminal mode token returned by terminal_enable_raw_mode()");
		RETURN_THROWS();
	}

	RETURN_BOOL(terminal_restore_stream_mode(&saved));
}

PHP_FUNCTION(terminal_read_key)
{
	double timeout = 0;
	bool timeout_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE_OR_NULL(timeout, timeout_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (!timeout_is_null && timeout < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	RETURN_FALSE;
}

PHP_MINIT_FUNCTION(terminal)
{
#if defined(ZTS) && defined(COMPILE_DL_TERMINAL)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	REGISTER_LONG_CONSTANT("TERMINAL_STDIN", TERMINAL_STREAM_STDIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TERMINAL_STDOUT", TERMINAL_STREAM_STDOUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TERMINAL_STDERR", TERMINAL_STREAM_STDERR, CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MINFO_FUNCTION(terminal)
{
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
	ext_functions,
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
