--TEST--
Terminal\Terminal::restoreMode rejects a token after successful restore
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
$mode = Terminal\Terminal::enableRawMode();
if (!$mode instanceof Terminal\ModeToken) {
    echo "raw-mode-unavailable\n";
    exit;
}

var_dump(Terminal\Terminal::restoreMode($mode));

try {
    Terminal\Terminal::restoreMode($mode);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
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
echo $status === 0 && $error === '' ? $output : $output . $error;
?>
--EXPECT--
bool(true)
Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be a terminal mode token returned by Terminal\Terminal::enableRawMode()
