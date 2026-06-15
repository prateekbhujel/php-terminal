--TEST--
Terminal\Terminal::getSize uses COLUMNS and LINES fallback
--EXTENSIONS--
terminal
--SKIPIF--
<?php
putenv('COLUMNS');
putenv('LINES');

if (Terminal\Terminal::getSize() !== false) {
    die('skip terminal size is available without environment fallback');
}
?>
--FILE--
<?php
putenv('COLUMNS=132');
putenv('LINES=43');

var_dump(Terminal\Terminal::getSize());

putenv('COLUMNS');
putenv('LINES');
?>
--EXPECT--
array(2) {
  ["cols"]=>
  int(132)
  ["rows"]=>
  int(43)
}
