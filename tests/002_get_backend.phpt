--TEST--
Terminal\Terminal::getBackend returns a Backend enum
--EXTENSIONS--
terminal
--FILE--
<?php
$backend = Terminal\Terminal::getBackend();
var_dump($backend instanceof Terminal\Backend);
?>
--EXPECT--
bool(true)
