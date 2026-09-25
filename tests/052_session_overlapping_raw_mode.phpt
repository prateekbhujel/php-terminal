--TEST--
Overlapping raw-mode sessions restore the original terminal mode in any order
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
$before = terminal_test_mode($input);

$a = Io\Terminal\Terminal::fromStreams($input);
$b = Io\Terminal\Terminal::fromStreams($input);

$a->enableRawMode();
$raw = terminal_test_mode($input);
$b->enableRawMode();

var_dump($a->restoreMode());
var_dump(terminal_test_mode($input) === $raw);
var_dump($b->restoreMode());
var_dump(terminal_test_mode($input) === $before);

$a->enableRawMode();
$b->enableRawMode();

var_dump($b->restoreMode());
var_dump(terminal_test_mode($input) === $raw);
var_dump($a->restoreMode());
var_dump(terminal_test_mode($input) === $before);

$a->enableRawMode();
$b->enableRawMode();
unset($a);

var_dump(terminal_test_mode($input) === $raw);
var_dump($b->restoreMode());
var_dump(terminal_test_mode($input) === $before);
PHP;

[$output, $error, $echo, $status, $restored] = terminal_test_pty($child, null, true);

echo $output;
var_dump($error === '', $echo === '', $status === 0, $restored);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
