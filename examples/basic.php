<?php

use Io\Terminal\ModeToken;
use Io\Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

$terminal = Terminal::create();
var_dump(Terminal::getBackend());
var_dump($terminal->isTty());
var_dump($terminal->supportsAnsi());
var_dump($terminal->enableAnsi());
$size = $terminal->getSize();
var_dump($size === false ? false : $size->toArray());
var_dump($terminal->write("hello from terminal\n"));

if (!stream_isatty(STDIN)) {
	$terminal->write("interactive demos: skipped, stdin is not a terminal\n");
	exit(0);
}

$mode = $terminal->enableRawMode();
var_dump($mode === false || $mode instanceof ModeToken);
if ($mode instanceof ModeToken) {
	try {
		var_dump($terminal->readKey(0.5));
	} finally {
		var_dump($terminal->restoreMode());
	}
}

$secret = $terminal->readSecret("Secret demo. Type a value and press Enter: ");
$terminal->write("\nSecret length: " . strlen($secret) . "\n");
