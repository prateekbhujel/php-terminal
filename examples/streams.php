<?php

use Terminal\Stream;
use Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

function yesno(bool $value): string
{
	return $value ? 'yes' : 'no';
}

$stdout = STDOUT;
$stderr = fopen('php://stderr', 'w');
$buffer = fopen('php://temp', 'w+');

$stdoutMatchesEnum = Terminal::isTty($stdout) === Terminal::isTty(Stream::Stdout);
$stderrIsTty = Terminal::isTty($stderr);
$ansiOnStdout = Terminal::supportsAnsi($stdout);
$bytesWritten = Terminal::write('buffered message', $buffer);

rewind($buffer);
$buffered = stream_get_contents($buffer);
$size = Terminal::getSize($stdout);

Terminal::write("terminal stream resource demo\n", $stdout);
Terminal::write('stdout enum matches resource: ' . yesno($stdoutMatchesEnum) . "\n", $stdout);
Terminal::write('stderr resource tty: ' . yesno($stderrIsTty) . "\n", $stdout);
Terminal::write('ansi on stdout: ' . yesno($ansiOnStdout) . "\n", $stdout);
Terminal::write('temp write bytes: ' . ($bytesWritten === false ? 'false' : (string) $bytesWritten) . "\n", $stdout);
Terminal::write("temp contents: {$buffered}\n", $stdout);
Terminal::write('size: ' . (is_array($size) ? "{$size['cols']}x{$size['rows']}" : 'unknown') . "\n", $stdout);
