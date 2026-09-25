--TEST--
POSIX readKey preserves a UTF-8 sequence across a sequence timeout
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

$extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
$code = <<<'PHP'
$input = fopen('php://fd/3', 'r+');
$terminal = Io\Terminal\Terminal::fromStreams($input);

echo "READY1\n";
$first = $terminal->readKey(0.20, 0.05);
echo 'FIRST|', $first === false ? 'false' : bin2hex($first), "\n";

echo "READY2\n";
$second = $terminal->readKey(1.0, 0.10);
echo 'SECOND|', $second === false ? 'false' : bin2hex($second), "\n";
PHP;

$child = proc_open(
    [PHP_BINARY, '-n', '-d', 'extension=' . $extension, '-r', $code],
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w'], 3 => ['pty']],
    $pipes
);

if (!is_resource($child)) {
    die("proc_open failed\n");
}

$before = terminal_test_mode($pipes[3]);
foreach ([1, 2, 3] as $fd) {
    stream_set_blocking($pipes[$fd], false);
}

$output = $error = '';
$sentLead = $sentContinuation = false;
$deadline = microtime(true) + (getenv('USE_ZEND_ALLOC') === '0' ? 60 : 5);

do {
    $output .= stream_get_contents($pipes[1]);
    $error .= stream_get_contents($pipes[2]);

    if (!$sentLead && str_contains($output, "READY1\n") && terminal_test_mode($pipes[3]) !== $before) {
        fwrite($pipes[3], "\xc3");
        $sentLead = true;
    }

    if (!$sentContinuation && str_contains($output, "READY2\n") && terminal_test_mode($pipes[3]) !== $before) {
        fwrite($pipes[3], "\xa9");
        $sentContinuation = true;
    }

    $status = proc_get_status($child);
    if (!$status['running']) {
        break;
    }

    usleep(10000);
} while (microtime(true) < $deadline);

if ($status['running']) {
    proc_terminate($child, 9);
    $error .= 'child timed out';
}

$output .= stream_get_contents($pipes[1]);
$error .= stream_get_contents($pipes[2]);
$restored = $before === terminal_test_mode($pipes[3]);

foreach ($pipes as $pipe) {
    fclose($pipe);
}

proc_close($child);

echo str_replace(["READY1\n", "READY2\n"], '', $output);
var_dump($error === '', $status['exitcode'] === 0, $restored);
?>
--EXPECT--
FIRST|false
SECOND|c3a9
bool(true)
bool(true)
bool(true)
