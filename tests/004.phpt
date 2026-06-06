--TEST--
Terminal raw mode helpers
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\ModeToken;
use Terminal\Stream;
use Terminal\Terminal;

try {
    Terminal::enableRawMode(Stream::Stdout);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    Terminal::restoreMode("bad-token");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$mode = Terminal::enableRawMode();
var_dump($mode === false || $mode instanceof ModeToken);
var_dump($mode === false || Terminal::restoreMode($mode));
?>
--EXPECTF--
Terminal\Terminal::enableRawMode(): Argument #1 ($stream) must be Terminal\Stream::Stdin
Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be of type Terminal\ModeToken, string given
bool(true)
bool(true)
