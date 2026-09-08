--TEST--
Terminal\Terminal::getColorDepth, supportsColor, and supportsTrueColor detect terminal color depth
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\ColorDepth;
use Terminal\Terminal;

// Verify enum backing values
var_dump(ColorDepth::None->value);
var_dump(ColorDepth::Standard->value);
var_dump(ColorDepth::Extended->value);
var_dump(ColorDepth::TrueColor->value);

// Reset environment
putenv('NO_COLOR');
putenv('COLORTERM');
putenv('TERM_PROGRAM');
putenv('TERM=dumb');

// 1. NO_COLOR forces ColorDepth::None
putenv('NO_COLOR=1');
var_dump(Terminal::getColorDepth() === ColorDepth::None);
var_dump(Terminal::supportsColor(ColorDepth::Standard));
var_dump(Terminal::supportsColor(ColorDepth::None));
var_dump(Terminal::supportsTrueColor());

// 2. CLICOLOR_FORCE with COLORTERM=truecolor
putenv('NO_COLOR');
putenv('CLICOLOR_FORCE=1');
putenv('COLORTERM=truecolor');
var_dump(Terminal::getColorDepth() === ColorDepth::TrueColor);
var_dump(Terminal::supportsTrueColor());
var_dump(Terminal::supportsColor(ColorDepth::TrueColor));
var_dump(Terminal::supportsColor(ColorDepth::Extended));
var_dump(Terminal::supportsColor(ColorDepth::Standard));

// 3. Modern terminal emulator (ghostty) supports TrueColor
putenv('COLORTERM');
putenv('TERM_PROGRAM=ghostty');
var_dump(Terminal::getColorDepth() === ColorDepth::TrueColor);
var_dump(Terminal::supportsTrueColor());

// 4. 256-color terminal
putenv('TERM_PROGRAM');
putenv('TERM=xterm-256color');
var_dump(Terminal::getColorDepth() === ColorDepth::Extended);
var_dump(Terminal::supportsTrueColor());
var_dump(Terminal::supportsColor(ColorDepth::Extended));
var_dump(Terminal::supportsColor(ColorDepth::Standard));

// 5. Standard 16-color ANSI terminal
putenv('TERM=vt100');
var_dump(Terminal::getColorDepth() === ColorDepth::Standard);
var_dump(Terminal::supportsTrueColor());
var_dump(Terminal::supportsColor(ColorDepth::Extended));
var_dump(Terminal::supportsColor(ColorDepth::Standard));
?>
--EXPECT--
int(0)
int(4)
int(8)
int(24)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
