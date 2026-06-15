--TEST--
Terminal runtime helpers
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Stream;
use Terminal\Terminal;

$size = Terminal::getSize();

var_dump(is_bool(Terminal::isTty()));
var_dump(is_bool(Terminal::supportsAnsi()));
var_dump(is_bool(Terminal::enableAnsi()));
var_dump($size === false || (is_array($size) && isset($size['cols'], $size['rows']) && is_int($size['cols']) && is_int($size['rows'])));
var_dump(Terminal::write("terminal-write\n") >= 15);

try {
    Terminal::write("x", Stream::Stdin);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    Terminal::enableAnsi(Stream::Stdin);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
terminal-write
bool(true)
Terminal\Terminal::write(): Argument #2 ($stream) must be Terminal\Stream::Stdout or Terminal\Stream::Stderr
Terminal\Terminal::enableAnsi(): Argument #1 ($stream) must be Terminal\Stream::Stdout or Terminal\Stream::Stderr
