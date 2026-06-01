<?php

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

function yn(bool $value): string
{
	return $value ? 'yes' : 'no';
}

$version = phpversion('terminal') ?: 'unknown';
$size = terminal_get_size();
$ansiBefore = terminal_supports_ansi();
$ansiEnabled = terminal_enable_ansi();
$ansiAfter = terminal_supports_ansi();

echo "terminal doctor\n";
echo "version: {$version}\n";
echo "backend: " . terminal_backend() . "\n";
echo "stdin tty: " . yn(terminal_is_tty(TERMINAL_STDIN)) . "\n";
echo "stdout tty: " . yn(terminal_is_tty(TERMINAL_STDOUT)) . "\n";
echo "stderr tty: " . yn(terminal_is_tty(TERMINAL_STDERR)) . "\n";
echo "ansi before enable: " . yn($ansiBefore) . "\n";
echo "ansi enable result: " . yn($ansiEnabled) . "\n";
echo "ansi after enable: " . yn($ansiAfter) . "\n";
echo "size: " . (is_array($size) ? "{$size['columns']}x{$size['rows']}" : 'unknown') . "\n";

if (!terminal_is_tty(TERMINAL_STDIN)) {
	echo "interactive checks: skipped, stdin is not a terminal\n";
	exit(0);
}

echo "\nPress any key within 5 seconds: ";
$key = terminal_read_key(5);
echo "\nkey: " . ($key === false ? 'timeout' : $key) . "\n";

echo "Secret check. Type a value and press Enter within 30 seconds: ";
$secret = terminal_read_secret(30);
echo "\nsecret result: " . ($secret === false ? 'timeout/abort' : strlen($secret) . ' bytes') . "\n";
