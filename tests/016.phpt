--TEST--
Terminal\Terminal::supportsAnsi detects Windows VT capability
--EXTENSIONS--
terminal
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') {
    die("skip Windows-only ANSI support test\n");
}

if (! extension_loaded('terminal')) {
    die("skip terminal extension is not loaded\n");
}

if (! Terminal\Terminal::isTty(Terminal\Stream::Stdout)) {
    die("skip stdout is not a Windows console\n");
}
?>
--FILE--
<?php
use Terminal\Stream;
use Terminal\Terminal;

putenv('NO_COLOR');

$supported = Terminal::supportsAnsi(Stream::Stdout);
$enabled = Terminal::enableAnsi(Stream::Stdout);

var_dump($supported === $enabled);
var_dump(Terminal::supportsAnsi(Stream::Stdout) === $enabled);

putenv('NO_COLOR=1');
var_dump(Terminal::supportsAnsi(Stream::Stdout));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
