--TEST--
Terminal\Terminal::supportsAnsi rejects dumb terminals
--EXTENSIONS--
terminal
--ENV--
TERM=dumb
--FILE--
<?php
var_dump(Terminal\Terminal::supportsAnsi(Terminal\Stream::Stdout));
?>
--EXPECT--
bool(false)
