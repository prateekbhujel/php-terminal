/* terminal extension for PHP (c) 2026 Pratik Bhujel */

#ifndef PHP_TERMINAL_H
# define PHP_TERMINAL_H

extern zend_module_entry terminal_module_entry;
# define phpext_terminal_ptr &terminal_module_entry

# define PHP_TERMINAL_VERSION "0.4.1"

# define TERMINAL_STREAM_STDIN 0
# define TERMINAL_STREAM_STDOUT 1
# define TERMINAL_STREAM_STDERR 2

# if defined(ZTS) && defined(COMPILE_DL_TERMINAL)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_TERMINAL_H */
