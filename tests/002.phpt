--TEST--
terminal backend and constants
--EXTENSIONS--
terminal
--FILE--
<?php
$backend = terminal_backend();
var_dump(in_array($backend, ['windows', 'posix'], true));
var_dump(TERMINAL_STDIN);
var_dump(TERMINAL_STDOUT);
var_dump(TERMINAL_STDERR);
var_dump(function_exists('terminal_enable_raw_mode'));
var_dump(function_exists('terminal_restore_mode'));
var_dump(function_exists('terminal_read_key'));
?>
--EXPECT--
bool(true)
int(0)
int(1)
int(2)
bool(true)
bool(true)
bool(true)
