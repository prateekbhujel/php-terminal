--TEST--
terminal runtime helpers
--EXTENSIONS--
terminal
--FILE--
<?php
$size = terminal_get_size();

var_dump(is_bool(terminal_is_tty()));
var_dump(is_bool(terminal_supports_ansi()));
var_dump($size === false || (is_array($size) && isset($size['columns'], $size['rows']) && is_int($size['columns']) && is_int($size['rows'])));
var_dump(terminal_write("terminal-write\n") >= 15);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
terminal-write
bool(true)
