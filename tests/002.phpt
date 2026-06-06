--TEST--
Terminal backend and enum cases
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Backend;
use Terminal\Key;
use Terminal\ModeToken;
use Terminal\Stream;
use Terminal\Terminal;

$backend = Terminal::getBackend();

var_dump($backend instanceof Backend);
var_dump(in_array($backend, [Backend::Windows, Backend::Posix], true));
var_dump(Stream::Stdin->value);
var_dump(Stream::Stdout->value);
var_dump(Stream::Stderr->value);
var_dump(Key::Up->value);
var_dump(Key::Enter->value);
var_dump(Key::Resize->value);
var_dump(class_exists(ModeToken::class));
var_dump(class_exists(Terminal::class));
var_dump(function_exists('terminal_backend'));
?>
--EXPECT--
bool(true)
bool(true)
int(0)
int(1)
int(2)
string(2) "up"
string(5) "enter"
string(6) "resize"
bool(true)
bool(true)
bool(false)
