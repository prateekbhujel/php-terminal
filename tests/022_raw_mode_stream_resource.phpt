--TEST--
Terminal raw mode accepts php://stdin and restores through its ModeToken
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
$extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
$code = <<<'PHP'
$stdin = fopen('php://stdin', 'r');
var_dump(Terminal\Terminal::isTty($stdin));
$mode = Terminal\Terminal::enableRawMode($stdin);
var_dump($mode instanceof Terminal\ModeToken);
unset($stdin);
var_dump(Terminal\Terminal::restoreMode($mode));
PHP;

$command = escapeshellarg(PHP_BINARY) . ' -n -d extension=' . escapeshellarg($extension) . ' -r ' . escapeshellarg($code);
$descriptors = [
    0 => ['pty'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];

$process = proc_open($command, $descriptors, $pipes);
if (!is_resource($process)) {
    die("proc_open failed\n");
}

$output = stream_get_contents($pipes[1]);
$error = stream_get_contents($pipes[2]);

foreach ($pipes as $pipe) {
    fclose($pipe);
}

$status = proc_close($process);
if ($status !== 0 || $error !== '') {
    echo $output, $error;
    exit;
}

echo $output;
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
