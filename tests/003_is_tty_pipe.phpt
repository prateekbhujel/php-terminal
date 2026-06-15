--TEST--
Terminal\Terminal::isTty detects piped stdin during phpt
--EXTENSIONS--
terminal
--FILE--
<?php
var_dump(Terminal\Terminal::isTty(Terminal\Stream::Stdin));
?>
--EXPECT--
bool(false)
