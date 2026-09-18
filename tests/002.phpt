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
var_dump(Stream::Stdin->name);
var_dump(Stream::Stdout->name);
var_dump(Stream::Stderr->name);
var_dump(Key::Up->name);
var_dump(Key::Enter->name);
var_dump(Key::Resize->name);
var_dump(class_exists(ModeToken::class));
var_dump(class_exists(Terminal::class));
var_dump(function_exists('terminal_backend'));
?>
--EXPECT--
bool(true)
bool(true)
string(5) "Stdin"
string(6) "Stdout"
string(6) "Stderr"
string(2) "Up"
string(5) "Enter"
string(6) "Resize"
bool(true)
bool(true)
bool(false)
