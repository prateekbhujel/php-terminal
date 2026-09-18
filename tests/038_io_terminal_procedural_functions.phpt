--TEST--
Io\Terminal free-standing procedural functions
--EXTENSIONS--
terminal
--ENV--
COLUMNS=80
LINES=25
--FILE--
<?php
namespace Io\Terminal;

var_dump(get_backend() instanceof Backend);
var_dump(is_bool(is_tty()));
var_dump(is_bool(supports_ansi()));
var_dump(is_bool(enable_ansi()));

$size = get_size();
var_dump(is_array($size) && $size['cols'] === 80 && $size['rows'] === 25);
var_dump(get_width() === 80);
var_dump(get_height() === 25);

$depth = get_color_depth();
var_dump($depth instanceof ColorDepth);
var_dump(is_bool(supports_true_color()));
var_dump(is_bool(supports_color(ColorDepth::None)));
var_dump(is_bool(beep()));
var_dump(is_bool(set_title('Test Title')));

$stream = fopen('php://temp', 'w+');
var_dump(write('hello', $stream) === 5);
rewind($stream);
var_dump(stream_get_contents($stream) === 'hello');
fclose($stream);
?>
--EXPECT--
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
bool(true)
bool(true)
bool(true)
bool(true)
