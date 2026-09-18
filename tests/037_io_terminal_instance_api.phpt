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

$stdout = Terminal::stdout();
$stderr = Terminal::stderr();
$stdin = Terminal::stdin();

var_dump($stdout instanceof Terminal);
var_dump($stdout->getStream() === Stream::Stdout);
var_dump($stderr->getStream() === Stream::Stderr);
var_dump($stdin->getStream() === Stream::Stdin);

$backend = Terminal::getBackend();
var_dump($backend instanceof Backend);

$custom = new Terminal(fopen('php://temp', 'w+'));
var_dump(is_resource($custom->getStream()));
var_dump($custom->isTty());
var_dump($custom->supportsAnsi());
var_dump($custom->write("test") === 4);

$size = $stdout->getSize();
var_dump(is_array($size) && $size['cols'] === 100 && $size['rows'] === 30);
var_dump($stdout->getWidth() === 100);
var_dump($stdout->getHeight() === 30);

$depth = $stdout->getColorDepth();
var_dump($depth instanceof ColorDepth);
var_dump(is_bool($stdout->supportsTrueColor()));
var_dump(is_bool($stdout->supportsColor(ColorDepth::None)));
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
