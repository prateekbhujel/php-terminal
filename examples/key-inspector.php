<?php

use Terminal\Key;
use Terminal\Stream;
use Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

if (!Terminal::isTty(Stream::Stdin)) {
	fwrite(STDERR, "stdin is not a terminal\n");
	exit(1);
}

Terminal::enableAnsi(Stream::Stdout);

function describe_key(Key|string|false $key): string
{
	if ($key === false) {
		return 'timeout';
	}

	if ($key instanceof Key) {
		return "enum {$key->name} ({$key->value})";
	}

	$display = json_encode($key, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);

	return "string {$display}, bytes=" . strlen($key) . ", hex=" . bin2hex($key);
}

Terminal::write("terminal key inspector\n");
Terminal::write("Press keys to see what the extension returns. Press Esc to exit.\n\n");

while (true) {
	$key = Terminal::readKey(30);

	Terminal::write(describe_key($key) . "\n");

	if ($key === Key::Escape) {
		break;
	}
}
