--TEST--
Session restore handles a token consumed outside its owner
--EXTENSIONS--
terminal
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip pseudo terminal test is POSIX only\n");
}

if (!function_exists('proc_open')) {
    die("skip proc_open is unavailable\n");
}

$descriptors = [
    0 => ['pty'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];

$process = @proc_open(escapeshellarg(PHP_BINARY) . ' -r ' . escapeshellarg('exit(0);'), $descriptors, $pipes);
if (!is_resource($process)) {
    die("skip pseudo terminal is unavailable\n");
}

foreach ($pipes as $pipe) {
    fclose($pipe);
}

proc_close($process);
?>
--FILE--
<?php
require __DIR__ . '/terminal_pty.inc';

$child = <<<'PHP'
$input = fopen('php://fd/3', 'r+');

$owner = Io\Terminal\Terminal::fromStreams($input);
$other = Io\Terminal\Terminal::fromStreams($input);

$token = $owner->enableRawMode();
var_dump($other->restoreMode($token));
var_dump($owner->restoreMode());

$token = $owner->enableRawMode();
var_dump(Terminal\Terminal::restoreMode($token));
var_dump($owner->restoreMode());
PHP;

[$output, $error, $echo, $status, $restored] = terminal_test_pty($child, null, true);

echo $output;
var_dump($error === '', $echo === '', $status === 0, $restored);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
