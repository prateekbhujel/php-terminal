/* terminal extension for PHP (c) 2026 Pratik Bhujel */

#ifndef PHP_TERMINAL_H
# define PHP_TERMINAL_H

extern zend_module_entry terminal_module_entry;
# define phpext_terminal_ptr &terminal_module_entry

# define PHP_TERMINAL_VERSION "1.0.0"

# define TERMINAL_STREAM_STDIN 0
# define TERMINAL_STREAM_STDOUT 1
# define TERMINAL_STREAM_STDERR 2

# if defined(ZTS) && defined(COMPILE_DL_TERMINAL)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

# if PHP_VERSION_ID < 80400
static zend_always_inline zend_class_entry *terminal_register_internal_class_with_flags(
	zend_class_entry *ce,
	zend_class_entry *parent_ce,
	uint32_t flags
) {
	zend_class_entry *entry = zend_register_internal_class_ex(ce, parent_ce);
	entry->ce_flags |= flags;
	return entry;
}
#  define zend_register_internal_class_with_flags terminal_register_internal_class_with_flags

#  undef ZEND_RAW_FENTRY
#  define ZEND_RAW_FENTRY(zend_name, name, arg_info, flags, ...) \
	{ zend_name, name, arg_info, (uint32_t) (sizeof(arg_info)/sizeof(struct _zend_internal_arg_info)-1), flags },
# endif

#endif	/* PHP_TERMINAL_H */
