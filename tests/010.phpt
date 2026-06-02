--TEST--
Terminal namespace is the only public API
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Backend;
use Terminal\Stream;
use Terminal\Terminal;

$oldFunctions = [
    'terminal_backend',
    'terminal_is_tty',
    'terminal_supports_ansi',
    'terminal_enable_ansi',
    'terminal_get_size',
    'terminal_write',
    'terminal_enable_raw_mode',
    'terminal_restore_mode',
    'terminal_read_key',
    'terminal_read_secret',
];

foreach ($oldFunctions as $function) {
    var_dump(function_exists($function));
}

foreach (['TERMINAL_STDIN', 'TERMINAL_STDOUT', 'TERMINAL_STDERR'] as $constant) {
    var_dump(defined($constant));
}

var_dump(Terminal::getBackend() instanceof Backend);
var_dump(Terminal::isTty(Stream::Stdout) === Terminal::isTty());
var_dump(Terminal::supportsAnsi(Stream::Stdout) === Terminal::supportsAnsi());

try {
    Terminal::enableAnsi(Stream::Stdin);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    Terminal::readKey(-1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
Terminal\Terminal::enableAnsi(): Argument #1 ($stream) must be Terminal\Stream::Stdout or Terminal\Stream::Stderr
Terminal\Terminal::readKey(): Argument #1 ($timeout) must be greater than or equal to 0
