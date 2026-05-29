--TEST--
terminal_read_key reads common keys from a pseudo terminal
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
function read_key_from_child(string $input, float $timeout): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = 'echo "ready\n"; var_dump(terminal_read_key(' . var_export($timeout, true) . '));';
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
    'printable' => ['a', 'string(1) "a"'],
    'enter' => ["\n", 'string(5) "enter"'],
    'tab' => ["\t", 'string(3) "tab"'],
    'backspace' => ["\x7f", 'string(9) "backspace"'],
    'escape' => ["\033", 'string(6) "escape"'],
    'up' => ["\033[A", 'string(2) "up"'],
    'down' => ["\033[B", 'string(4) "down"'],
    'right' => ["\033[C", 'string(5) "right"'],
    'left' => ["\033[D", 'string(4) "left"'],
    'home' => ["\033[H", 'string(4) "home"'],
    'end' => ["\033[F", 'string(3) "end"'],
    'delete' => ["\033[3~", 'string(6) "delete"'],
    'pageup' => ["\033[5~", 'string(6) "pageup"'],
    'pagedown' => ["\033[6~", 'string(8) "pagedown"'],
];

foreach ($cases as $name => [$input, $expected]) {
    $output = read_key_from_child($input, 1.0);
    echo str_contains($output, $expected) ? "$name\n" : $output;
}

$timeout = read_key_from_child('', 0.05);
echo str_contains($timeout, 'bool(false)') ? "timeout\n" : $timeout;
?>
--EXPECT--
printable
enter
tab
backspace
escape
up
down
right
left
home
end
delete
pageup
pagedown
timeout
