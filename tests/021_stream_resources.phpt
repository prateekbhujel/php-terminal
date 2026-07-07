--TEST--
Terminal stream-aware methods accept PHP stream resources
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Stream;
use Terminal\Terminal;

$stream = fopen('php://temp', 'w+');

putenv('NO_COLOR=1');
putenv('COLUMNS=91');
putenv('LINES=37');

var_dump(Terminal::isTty($stream));
var_dump(Terminal::supportsAnsi($stream));
var_dump(Terminal::enableAnsi($stream));
var_dump(Terminal::write('hello', $stream));

rewind($stream);
var_dump(stream_get_contents($stream));
var_dump(Terminal::getSize($stream));
var_dump(Terminal::isTty(STDOUT) === Terminal::isTty(Stream::Stdout));
var_dump(Terminal::enableRawMode($stream));

try {
    Terminal::isTty(1);
} catch (TypeError $error) {
    echo $error->getMessage(), "\n";
}

fclose($stream);

try {
    Terminal::isTty($stream);
} catch (TypeError $error) {
    echo "closed stream rejected\n";
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
int(5)
string(5) "hello"
array(2) {
  ["cols"]=>
  int(91)
  ["rows"]=>
  int(37)
}
bool(true)
bool(false)
Terminal\Terminal::isTty(): Argument #1 ($stream) must be of type Terminal\Stream|resource, int given
closed stream rejected
