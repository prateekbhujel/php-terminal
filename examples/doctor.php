<?php

use Terminal\Key;
use Terminal\Stream;
use Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

function yn(bool $value): string
{
	return $value ? 'yes' : 'no';
}

$version = phpversion('terminal') ?: 'unknown';
$size = Terminal::getSize();
$ansiBefore = Terminal::supportsAnsi();
$ansiEnabled = Terminal::enableAnsi();
$ansiAfter = Terminal::supportsAnsi();

echo "terminal doctor\n";
echo "version: {$version}\n";
echo "backend: " . Terminal::getBackend()->value . "\n";
echo "stdin tty: " . yn(Terminal::isTty(Stream::Stdin)) . "\n";
echo "stdout tty: " . yn(Terminal::isTty(Stream::Stdout)) . "\n";
echo "stderr tty: " . yn(Terminal::isTty(Stream::Stderr)) . "\n";
echo "ansi before enable: " . yn($ansiBefore) . "\n";
echo "ansi enable result: " . yn($ansiEnabled) . "\n";
echo "ansi after enable: " . yn($ansiAfter) . "\n";
echo "size: " . (is_array($size) ? "{$size['cols']}x{$size['rows']}" : 'unknown') . "\n";

if (!Terminal::isTty(Stream::Stdin)) {
	echo "interactive checks: skipped, stdin is not a terminal\n";
	exit(0);
}

echo "\nPress any key within 5 seconds: ";
$key = Terminal::readKey(5);
echo "\nkey: " . ($key === false ? 'timeout' : ($key instanceof Key ? $key->value : $key)) . "\n";

$secret = Terminal::readSecret("Secret check. Type a value and press Enter: ");
echo "\nsecret result: " . strlen($secret) . " bytes\n";
