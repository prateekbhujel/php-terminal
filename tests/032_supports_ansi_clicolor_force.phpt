--TEST--
Terminal\Terminal::supportsAnsi honors CLICOLOR_FORCE and NO_COLOR precedence
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Stream;
use Terminal\Terminal;

// Ensure base environment has no color overrides
putenv('NO_COLOR');
putenv('COLORTERM');
putenv('TERM=dumb');

// 1. Piped/non-TTY stream with CLICOLOR_FORCE=1 forces ANSI support
putenv('CLICOLOR_FORCE=1');
var_dump(Terminal::supportsAnsi(Stream::Stdout));

// 2. NO_COLOR takes precedence over CLICOLOR_FORCE
putenv('NO_COLOR=1');
var_dump(Terminal::supportsAnsi(Stream::Stdout));

// 3. CLICOLOR_FORCE=0 does not force ANSI
putenv('NO_COLOR');
putenv('CLICOLOR_FORCE=0');
var_dump(Terminal::supportsAnsi(Stream::Stdout));

// 4. Unset CLICOLOR_FORCE does not force ANSI
putenv('CLICOLOR_FORCE');
var_dump(Terminal::supportsAnsi(Stream::Stdout));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
