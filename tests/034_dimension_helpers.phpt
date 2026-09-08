--TEST--
Terminal\Terminal::getWidth and Terminal\Terminal::getHeight return dimensions
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Stream;
use Terminal\Terminal;

putenv('COLUMNS=120');
putenv('LINES=40');

var_dump(Terminal::getWidth());
var_dump(Terminal::getHeight());

$stream = fopen('php://temp', 'w+');
var_dump(Terminal::getWidth($stream));
var_dump(Terminal::getHeight($stream));
fclose($stream);

putenv('COLUMNS=80');
putenv('LINES=25');

var_dump(Terminal::getWidth(Stream::Stderr));
var_dump(Terminal::getHeight(Stream::Stderr));

$size = Terminal::getSize();
var_dump(Terminal::getWidth() === $size['cols']);
var_dump(Terminal::getHeight() === $size['rows']);
?>
--EXPECT--
int(120)
int(40)
int(120)
int(40)
int(80)
int(25)
bool(true)
bool(true)
