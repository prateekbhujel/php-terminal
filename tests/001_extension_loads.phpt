--TEST--
Terminal extension loads expected OOP symbols
--EXTENSIONS--
terminal
--FILE--
<?php
var_dump(class_exists(Terminal\Terminal::class));
var_dump(class_exists(Terminal\ModeToken::class));
var_dump(enum_exists(Terminal\Key::class));
var_dump(enum_exists(Terminal\Stream::class));
var_dump(enum_exists(Terminal\Backend::class));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
