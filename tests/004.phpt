--TEST--
terminal raw mode helpers
--EXTENSIONS--
terminal
--FILE--
<?php
try {
    terminal_enable_raw_mode(TERMINAL_STDOUT);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    terminal_restore_mode("bad-token");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$mode = terminal_enable_raw_mode();
var_dump($mode === false || is_string($mode));
var_dump($mode === false || terminal_restore_mode($mode));
?>
--EXPECTF--
terminal_enable_raw_mode(): Argument #1 ($stream) must be TERMINAL_STDIN
terminal_restore_mode(): Argument #1 ($mode) must be a terminal mode token returned by terminal_enable_raw_mode()
bool(true)
bool(true)
