--TEST--
Terminal\Terminal::readKey rejects NAN for timeout and sequenceTimeout
--EXTENSIONS--
terminal
--FILE--
<?php
try {
    Terminal\Terminal::readKey(NAN);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    Terminal\Terminal::readKey(null, NAN);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Terminal\Terminal::readKey(): Argument #1 ($timeout) must be greater than or equal to 0
Terminal\Terminal::readKey(): Argument #2 ($sequenceTimeout) must be greater than or equal to 0
