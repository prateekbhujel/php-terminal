--TEST--
Terminal\Terminal::setTitle and Terminal\Terminal::beep behavior and sanitization
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Stream;
use Terminal\Terminal;

// 1. Sanitization: control characters are rejected to prevent escape injection
var_dump(Terminal::setTitle("Hello\nWorld"));
var_dump(Terminal::setTitle("Hello\rWorld"));
var_dump(Terminal::setTitle("Hello\033]0;pwned\007"));
var_dump(Terminal::setTitle("Hello\x07World"));

// 2. Setting title on non-TTY stream without ANSI capability returns false
putenv('NO_COLOR=1');
putenv('CLICOLOR_FORCE');
$stream = fopen('php://temp', 'w+');
var_dump(Terminal::setTitle("My Title", $stream));
var_dump(Terminal::beep($stream));
fclose($stream);

// 3. Setting title with ANSI capability emits standard OSC 0 sequence
putenv('NO_COLOR');
putenv('CLICOLOR_FORCE=1');
$stream = fopen('php://temp', 'w+');
var_dump(Terminal::setTitle("CLI Worker [active]", $stream));
rewind($stream);
$output = stream_get_contents($stream);
var_dump($output === "\033]0;CLI Worker [active]\x07");
fclose($stream);

// 4. Closed stream rejection
$stream = fopen('php://temp', 'w+');
fclose($stream);
try {
    Terminal::setTitle("Test", $stream);
} catch (TypeError $error) {
    echo "closed stream rejected\n";
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
closed stream rejected
