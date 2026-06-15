--TEST--
Terminal\Terminal::readKey manual F1-F12 check
--EXTENSIONS--
terminal
--SKIPIF--
<?php
die("skip manual-only: run this test in a real terminal and press F1-F12 to verify Terminal\\Key::F1 through Terminal\\Key::F12\n");
?>
--FILE--
<?php
// Manual test only:
// Run this in a real terminal, press F1 through F12, and verify readKey()
// returns Terminal\Key::F1 through Terminal\Key::F12.
?>
--EXPECT--
