--TEST--
Terminal\Terminal::restoreMode rejects non-token values
--EXTENSIONS--
terminal
--FILE--
<?php
try {
    Terminal\Terminal::restoreMode("not-a-token");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be of type Terminal\ModeToken, string given
