--TEST--
Terminal\Terminal::supportsAnsi honors empty NO_COLOR
--EXTENSIONS--
terminal
--ENV--
TERM=xterm-256color
NO_COLOR=
--FILE--
<?php
var_dump(Terminal\Terminal::supportsAnsi(Terminal\Stream::Stdout));
?>
--EXPECT--
bool(false)
