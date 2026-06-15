<?php

use Terminal\ModeToken;
use Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

var_dump(Terminal::getBackend());
var_dump(Terminal::isTty());
var_dump(Terminal::supportsAnsi());
var_dump(Terminal::enableAnsi());
var_dump(Terminal::getSize());
var_dump(Terminal::write("hello from terminal\n"));

$mode = Terminal::enableRawMode();
var_dump($mode === false || $mode instanceof ModeToken);
if ($mode instanceof ModeToken) {
	try {
		var_dump(Terminal::readKey(0.5));
	} finally {
		var_dump(Terminal::restoreMode($mode));
	}
}

$secret = Terminal::readSecret("Secret demo. Type a value and press Enter: ");
Terminal::write("\nSecret length: " . strlen($secret) . "\n");
