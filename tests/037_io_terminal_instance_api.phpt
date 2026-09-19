--TEST--
Io\Terminal\Terminal instance methods
--EXTENSIONS--
terminal
--ENV--
COLUMNS=100
LINES=30
--FILE--
<?php
use Io\Terminal\Backend;
use Io\Terminal\ColorDepth;
use Io\Terminal\Stream;
use Io\Terminal\Terminal;

$terminal = Terminal::create();
$opened = Terminal::open();

var_dump($terminal instanceof Terminal);
var_dump($opened instanceof Terminal);
var_dump($terminal->getInputStream() === Stream::Stdin);
var_dump($terminal->getOutputStream() === Stream::Stdout);

$backend = Terminal::getBackend();
var_dump($backend instanceof Backend);

$temp = fopen('php://temp', 'w+');
$custom = Terminal::fromStream($temp);
var_dump(is_resource($custom->getStream()));
var_dump($custom->isTty());
var_dump($custom->supportsAnsi());
var_dump($custom->write("test") === 4);

$size = $terminal->getSize();
var_dump($size instanceof \Io\Terminal\TerminalSize);
var_dump($size->cols === 100);
var_dump($size->rows === 30);
var_dump($size->width === 100);
var_dump($size->height === 30);
var_dump($size->toArray() === ['cols' => 100, 'rows' => 30]);

$depth = $terminal->getColorDepth();
var_dump($depth instanceof ColorDepth);
var_dump(is_bool($terminal->supportsTrueColor()));
var_dump(is_bool($terminal->supportsColor(ColorDepth::None)));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
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
bool(true)
