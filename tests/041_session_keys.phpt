--TEST--
Session key reads use a custom TTY and preserve both temporary and RAII raw modes
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
use Io\Terminal\Terminal;
$input = fopen('php://fd/3', 'r+');
$term = Terminal::fromStreams($input);
$before = terminal_test_mode($input);
echo "READY\n";
$key = $term->readKey(1.0, 0.1);
echo $key instanceof Io\Terminal\Key ? $key->name : bin2hex($key);
echo terminal_test_mode($input) === $before ? ":restored\n" : ":wrong-mode\n";
$token = $term->enableRawMode();
$raw = terminal_test_mode($input);
var_dump($term->readKey(0.01));
echo terminal_test_mode($input) === $raw ? "still-raw\n" : "wrong-mode\n";
unset($term);
echo terminal_test_mode($input) === $before ? "destructor-restored\n" : "wrong-mode\n";
PHP;
foreach (["\x1b[A", "\xc3\xa9"] as $input) {
    [$output, $error, $echo, $status] = terminal_test_pty($child, $input);
    echo $output;
    var_dump($error === '', $echo === '', $status === 0);
}
?>
--EXPECT--
Up:restored
bool(false)
still-raw
destructor-restored
bool(true)
bool(true)
bool(true)
c3a9:restored
bool(false)
still-raw
destructor-restored
bool(true)
bool(true)
bool(true)
