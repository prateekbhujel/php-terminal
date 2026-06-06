--TEST--
Terminal\Terminal::supportsAnsi honors NO_COLOR
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
function supports_ansi_with_env(array $env): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = <<<'PHP'
var_dump(Terminal\Terminal::supportsAnsi(Terminal\Stream::Stdin));
PHP;
    $command = escapeshellarg(PHP_BINARY) . ' -n -d extension=' . escapeshellarg($extension) . ' -r ' . escapeshellarg($code);
    $descriptors = [
        0 => ['pty'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
    ];

    $process = proc_open($command, $descriptors, $pipes, null, $env);
    if (!is_resource($process)) {
        return 'proc_open failed';
    }

    $output = stream_get_contents($pipes[1]);
    $error = stream_get_contents($pipes[2]);

    foreach ($pipes as $pipe) {
        fclose($pipe);
    }

    $status = proc_close($process);
    if ($status !== 0 || $error !== '') {
        return $output . $error;
    }

    return $output;
}

echo supports_ansi_with_env(['TERM' => 'xterm-256color']);
echo supports_ansi_with_env(['TERM' => 'xterm-256color', 'NO_COLOR' => '1']);
?>
--EXPECT--
bool(true)
bool(false)
