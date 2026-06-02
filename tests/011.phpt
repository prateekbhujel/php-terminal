--TEST--
Terminal\Terminal::readKey returns enum cases for special keys
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
function read_key_from_child(string $input): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = <<<'PHP'
echo "ready\n";
$key = Terminal\Terminal::readKey(1.0);
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
        return 'proc_open failed';
    }

    stream_set_blocking($pipes[1], false);
    $output = '';
    $start = microtime(true);

    while (microtime(true) - $start < 2 && !str_contains($output, "ready\n")) {
        $output .= stream_get_contents($pipes[1]);
        usleep(10000);
    }

    fwrite($pipes[0], $input);

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
    'printable' => ['a', 'string(1) "a"'],
    'up' => ["\033[A", "Up:up"],
    'enter' => ["\n", "Enter:enter"],
    'backspace' => ["\x7f", "Backspace:backspace"],
];

foreach ($cases as $name => [$input, $expected]) {
    $output = read_key_from_child($input);
    echo str_contains($output, $expected) ? "$name\n" : $output;
}
?>
--EXPECT--
printable
up
enter
backspace
