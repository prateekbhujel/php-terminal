--TEST--
Repeated raw mode on one session preserves the original mode and single-use token
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
$term = Io\Terminal\Terminal::fromStreams($input);
$first = $term->enableRawMode();
$raw = terminal_test_mode($input);
$second = $term->enableRawMode();
var_dump($first === $second, terminal_test_mode($input) === $raw);
var_dump($term->restoreMode(), terminal_test_mode($input) === $before);
var_dump($term->restoreMode());
$token = $term->enableRawMode();
unset($term);
var_dump(terminal_test_mode($input) === $before);
try { Terminal\Terminal::restoreMode($token); } catch (ValueError $e) { echo "single-use\n"; }
PHP;
[$output, $error, $echo, $status] = terminal_test_pty($child, null);
echo $output;
var_dump($error === '', $echo === '', $status === 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
single-use
bool(true)
bool(true)
bool(true)
