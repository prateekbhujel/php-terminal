--TEST--
terminal_read_key API shape
--EXTENSIONS--
terminal
--FILE--
<?php
$key = terminal_read_key(0);
var_dump($key === false || is_string($key));

try {
    terminal_read_key(-1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
terminal_read_key(): Argument #1 ($timeout) must be greater than or equal to 0
