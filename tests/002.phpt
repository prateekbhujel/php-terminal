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
?>
--EXPECT--
bool(true)
int(0)
int(1)
int(2)
