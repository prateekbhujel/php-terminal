--TEST--
Terminal\Terminal::readKey API shape
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Key;
use Terminal\Terminal;

$key = Terminal::readKey(0);
var_dump($key === false || $key instanceof Key || is_string($key));

try {
    Terminal::readKey(-1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
Terminal\Terminal::readKey(): Argument #1 ($timeout) must be greater than or equal to 0
