--TEST--
examples/basic.php skips interactive demos when stdin is not a terminal
--EXTENSIONS--
terminal
--ENV--
NO_COLOR=1
COLUMNS=120
LINES=40
--FILE--
<?php
include dirname(__DIR__) . '/examples/basic.php';
?>
--EXPECTF--
enum(Terminal\Backend::%s)
bool(false)
bool(false)
bool(false)
array(2) {
  ["cols"]=>
  int(120)
  ["rows"]=>
  int(40)
}
hello from terminal
int(20)
interactive demos: skipped, stdin is not a terminal
