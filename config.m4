PHP_ARG_ENABLE([terminal],
  [whether to enable terminal support],
  [AS_HELP_STRING([--enable-terminal],
    [Enable terminal support])],
  [no])

AS_VAR_IF([PHP_TERMINAL], [no],, [
  AC_DEFINE([HAVE_TERMINAL], [1],
    [Define to 1 if the PHP extension 'terminal' is available.])

  PHP_NEW_EXTENSION([terminal],
    [terminal.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
