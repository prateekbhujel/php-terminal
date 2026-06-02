--TEST--
Terminal\Terminal raw mode preserves output processing
--EXTENSIONS--
terminal
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip stty output processing test is POSIX only\n");
}

if (!function_exists('proc_open')) {
    die("skip proc_open is unavailable\n");
}

if (!function_exists('shell_exec')) {
    die("skip shell_exec is unavailable\n");
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
$mode = Terminal\Terminal::enableRawMode();
if (!is_string($mode)) {
    echo "raw-mode-unavailable\n";
    exit;
}

$output = shell_exec('stty -a <&0 2>&1');
Terminal\Terminal::restoreMode($mode);
echo $output;
PHP;

$command = escapeshellarg(PHP_BINARY) . ' -n -d extension=' . escapeshellarg($extension) . ' -r ' . escapeshellarg($code);
$descriptors = [
    0 => ['pty'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];

$process = proc_open($command, $descriptors, $pipes);
if (!is_resource($process)) {
    echo "proc_open failed\n";
    exit;
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

$opost = preg_match('/(^|[;\s])opost([;\s]|$)/', $output) === 1;
$disabledOpost = preg_match('/(^|[;\s])-opost([;\s]|$)/', $output) === 1;

echo $opost && ! $disabledOpost ? "opost-preserved\n" : $output;
?>
--EXPECT--
opost-preserved
