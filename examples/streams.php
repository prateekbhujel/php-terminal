<?php

use Io\Terminal\Stream;
use Io\Terminal\Terminal;

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

$terminal = Terminal::fromStreams(STDIN, $stdout);
$stderrTerminal = Terminal::fromStream($stderr);
$bufferTerminal = Terminal::fromStreams(STDIN, $buffer);

$stdoutMatchesEnum = $terminal->isTty() === Terminal::fromStreams(Stream::Stdin, Stream::Stdout)->isTty();
$stderrIsTty = $stderrTerminal->isTty();
$ansiOnStdout = $terminal->supportsAnsi();
$bytesWritten = $bufferTerminal->write('buffered message');

rewind($buffer);
$buffered = stream_get_contents($buffer);
$size = $terminal->getSize();

$terminal->write("terminal stream resource demo\n");
$terminal->write('stdout enum matches resource: ' . yesno($stdoutMatchesEnum) . "\n");
$terminal->write('stderr resource tty: ' . yesno($stderrIsTty) . "\n");
$terminal->write('ansi on stdout: ' . yesno($ansiOnStdout) . "\n");
$terminal->write('temp write bytes: ' . ($bytesWritten === false ? 'false' : (string) $bytesWritten) . "\n");
$terminal->write("temp contents: {$buffered}\n");
$terminal->write('size: ' . ($size !== false ? "{$size->cols}x{$size->rows}" : 'unknown') . "\n");
