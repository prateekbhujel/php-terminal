<?php

use Io\Terminal\Key;
use Io\Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

function yn(bool $value): string
{
	return $value ? 'yes' : 'no';
}

$terminal = Terminal::create();
$version = phpversion('terminal') ?: 'unknown';
$size = $terminal->getSize();
$ansiBefore = $terminal->supportsAnsi();
$ansiEnabled = $terminal->enableAnsi();
$ansiAfter = $terminal->supportsAnsi();

echo "terminal doctor\n";
echo "version: {$version}\n";
echo "backend: " . Terminal::getBackend()->name . "\n";
echo "stdin tty: " . yn(stream_isatty(STDIN)) . "\n";
echo "stdout tty: " . yn(stream_isatty(STDOUT)) . "\n";
echo "stderr tty: " . yn(stream_isatty(STDERR)) . "\n";
echo "ansi before enable: " . yn($ansiBefore) . "\n";
echo "ansi enable result: " . yn($ansiEnabled) . "\n";
echo "ansi after enable: " . yn($ansiAfter) . "\n";
echo "size: " . ($size !== false ? "{$size->cols}x{$size->rows}" : 'unknown') . "\n";

if (!stream_isatty(STDIN)) {
	echo "interactive checks: skipped, stdin is not a terminal\n";
	exit(0);
}

echo "\nPress any key within 5 seconds: ";
$key = $terminal->readKey(5);
echo "\nkey: " . ($key === false ? 'timeout' : ($key instanceof Key ? $key->name : $key)) . "\n";

$secret = $terminal->readSecret("Secret check. Type a value and press Enter: ");
echo "\nsecret result: " . strlen($secret) . " bytes\n";
