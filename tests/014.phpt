--TEST--
Terminal\Terminal::readKey surfaces SIGWINCH as resize
--EXTENSIONS--
terminal
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip SIGWINCH test is POSIX only\n");
}

if (!function_exists('proc_open')) {
    die("skip proc_open is unavailable\n");
}

if (!function_exists('exec')) {
    die("skip exec is unavailable\n");
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
echo "ready:" . getmypid() . "\n";
$key = Terminal\Terminal::readKey(2.0);
if ($key instanceof Terminal\Key) {
    echo $key->name . ':' . $key->value . "\n";
} else {
    var_dump($key);
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

stream_set_blocking($pipes[1], false);
$output = '';
$start = microtime(true);
$pid = null;

while (microtime(true) - $start < 2) {
    $output .= stream_get_contents($pipes[1]);
    if (preg_match('/ready:(\d+)/', $output, $matches)) {
        $pid = (int) $matches[1];
        break;
    }
    usleep(10000);
}

if ($pid === null) {
    echo $output;
    exit;
}

exec('kill -WINCH ' . $pid);

stream_set_blocking($pipes[1], true);
$output .= stream_get_contents($pipes[1]);
$error = stream_get_contents($pipes[2]);

foreach ($pipes as $pipe) {
    fclose($pipe);
}

$status = proc_close($process);
if ($status !== 0 || $error !== '') {
    echo $output, $error;
    exit;
}

echo str_contains($output, 'Resize:resize') ? "resize\n" : $output;
?>
--EXPECT--
resize
