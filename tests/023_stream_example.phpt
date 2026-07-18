--TEST--
examples/streams.php runs without interactive input
--EXTENSIONS--
terminal
--ENV--
NO_COLOR=1
COLUMNS=120
LINES=40
--FILE--
<?php
include dirname(__DIR__) . '/examples/streams.php';
?>
--EXPECT--
terminal stream resource demo
stdout enum matches resource: yes
stderr resource tty: no
ansi on stdout: no
temp write bytes: 16
temp contents: buffered message
size: 120x40
