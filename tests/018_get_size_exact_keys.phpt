--TEST--
Terminal\Terminal::getSize returns exactly 'cols' and 'rows' keys
--EXTENSIONS--
terminal
--ENV--
COLUMNS=132
LINES=43
--FILE--
<?php
/**
 * The 0.5 changelog changed getSize() to return 'cols' and 'rows'
 * (not 'columns'/'lines' or any other names).  This test pins the
 * exact key names and verifies the values match the env fallback.
 */
$size = Terminal\Terminal::getSize();

var_dump(is_array($size));

// Exact key set — no extra keys, no old names.
$keys = array_keys($size);
sort($keys);
var_dump($keys === ['cols', 'rows']);

var_dump($size['cols'] === 132);
var_dump($size['rows'] === 43);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
