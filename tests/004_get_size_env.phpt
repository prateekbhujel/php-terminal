--TEST--
Terminal\Terminal::getSize uses COLUMNS and LINES
--EXTENSIONS--
terminal
--ENV--
COLUMNS=80
LINES=24
--FILE--
<?php
$size = Terminal\Terminal::getSize();
var_dump(is_array($size));
var_dump(isset($size['cols'], $size['rows']));
var_dump($size['cols'] > 0);
var_dump($size['rows'] > 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
