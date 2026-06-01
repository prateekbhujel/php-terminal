--TEST--
terminal_read_secret reads hidden input from a pseudo terminal
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
function read_secret_from_child(string $input, float $timeout = 1.0): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = <<<'PHP'
echo "ready\n";
$secret = terminal_read_secret(%s);
var_dump($secret === false ? false : bin2hex($secret));
PHP;
    $command = escapeshellarg(PHP_BINARY) . ' -n -d extension=' . escapeshellarg($extension) . ' -r ' . escapeshellarg(sprintf($code, var_export($timeout, true)));
    $descriptors = [
        0 => ['pty'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
    ];

    $process = proc_open($command, $descriptors, $pipes);
    if (!is_resource($process)) {
        return 'proc_open failed';
    }

    stream_set_blocking($pipes[1], false);
    $output = '';
    $start = microtime(true);

    while (microtime(true) - $start < 2 && !str_contains($output, "ready\n")) {
        $output .= stream_get_contents($pipes[1]);
        usleep(10000);
    }

    if ($input !== '') {
        fwrite($pipes[0], $input);
    }

    stream_set_blocking($pipes[1], true);
    $output .= stream_get_contents($pipes[1]);
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

$cases = [
    'ascii' => ["secret\n", 'string(12) "736563726574"'],
    'backspace' => ["ab\x7fc\n", 'string(4) "6163"'],
    'utf8' => ["caf\xc3\xa9\n", 'string(10) "636166c3a9"'],
];

foreach ($cases as $name => [$input, $expected]) {
    $output = read_secret_from_child($input);
    echo str_contains($output, $expected) ? "$name\n" : $output;
}

$timeout = read_secret_from_child('', 0.05);
echo str_contains($timeout, 'bool(false)') ? "timeout\n" : $timeout;

try {
    terminal_read_secret(-1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ascii
backspace
utf8
timeout
terminal_read_secret(): Argument #1 ($timeout) must be greater than or equal to 0
