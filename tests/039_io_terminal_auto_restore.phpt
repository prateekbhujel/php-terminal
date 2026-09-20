--TEST--
Io\Terminal\Terminal restores raw mode on destruction and uncaught exception shutdown
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
$term = Io\Terminal\Terminal::fromStreams($input);
$token = $term->enableRawMode();
if ($token === false) {
    throw new LogicException('Cannot enable raw mode');
}
echo "RAW\n";
PHP;
foreach (['unset($term);', "throw new RuntimeException('uncaught');"] as $exit) {
    [$output, $error, $echo, $status, $restored] = terminal_test_pty($child . $exit, null, true);
    var_dump(str_contains($output, "RAW\n"), $restored, $echo === '');
    if ($status === 0) {
        var_dump($error === '');
    } else {
        var_dump($status === 255 && str_contains($output . $error, 'Uncaught RuntimeException: uncaught'));
    }
}
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
