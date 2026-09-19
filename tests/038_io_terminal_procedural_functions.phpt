--TEST--
Io\Terminal pure OO architecture and TerminalSize value object
--EXTENSIONS--
terminal
--ENV--
COLUMNS=80
LINES=25
--FILE--
<?php
namespace Io\Terminal;

// Verify procedural functions are eliminated (pure OO architecture)
var_dump(function_exists('Io\Terminal\is_tty'));
var_dump(function_exists('Io\Terminal\get_size'));
var_dump(function_exists('Io\Terminal\read_key'));

// Verify TerminalSize value object
$size = new TerminalSize(80, 25);
var_dump($size->cols === 80);
var_dump($size->rows === 25);
var_dump($size->width === 80);
var_dump($size->height === 25);
var_dump($size->toArray() === ['cols' => 80, 'rows' => 25]);

// Verify Terminal::fromStreams with input and output
$in = fopen('php://temp', 'w+');
$out = fopen('php://temp', 'w+');
$term = Terminal::fromStreams($in, $out);
var_dump($term->getInputStream() === $in);
var_dump($term->getOutputStream() === $out);
var_dump($term->write('hello') === 5);
rewind($out);
var_dump(stream_get_contents($out) === 'hello');
fclose($in);
fclose($out);
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
