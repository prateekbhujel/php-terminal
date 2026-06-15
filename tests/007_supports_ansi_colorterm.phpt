--TEST--
Terminal\Terminal::supportsAnsi accepts COLORTERM truecolor on a TTY
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
$code = 'var_dump(Terminal\Terminal::supportsAnsi(Terminal\Stream::Stdin));';
$command = escapeshellarg(PHP_BINARY) . ' -n -d extension=' . escapeshellarg($extension) . ' -r ' . escapeshellarg($code);
$descriptors = [
    0 => ['pty'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];
$process = proc_open($command, $descriptors, $pipes, null, ['COLORTERM' => 'truecolor']);
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
echo $status === 0 && $error === '' ? $output : $output . $error;
?>
--EXPECT--
bool(true)
