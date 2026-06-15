--TEST--
Terminal\Terminal::enableRawMode returns a ModeToken on a TTY
--EXTENSIONS--
terminal
--SKIPIF--
<?php
if (!Terminal\Terminal::isTty(Terminal\Stream::Stdin)) {
    die("skip stdin is not a terminal\n");
}
?>
--FILE--
<?php
$token = Terminal\Terminal::enableRawMode();
var_dump($token instanceof Terminal\ModeToken);
var_dump(Terminal\Terminal::restoreMode($token));
?>
--EXPECT--
bool(true)
bool(true)
