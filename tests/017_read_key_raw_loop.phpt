--TEST--
Terminal\Terminal::readKey survives repeated calls in a raw-mode loop
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
/**
 * Verifies that the raw-mode guard works correctly on POSIX:
 * calling readKey() multiple times in a loop does not accidentally restore
 * cooked mode between calls, so each subsequent read still works.
 *
 * We spin up a child that reads 3 keys in sequence and prints each one.
 * If the guard is broken the second or third read would block forever or
 * return garbage because the terminal would be back in canonical mode.
 */
function read_keys_from_child(array $inputs): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;

    $count = count($inputs);
    $code = <<<'PHP'
echo "ready\n";
for ($i = 0; $i < %d; $i++) {
    $key = Terminal\Terminal::readKey(2.0, 0.05);
    if ($key instanceof Terminal\Key) {
        echo $key->name . ':' . $key->value . "\n";
    } else {
        var_dump($key);
    }
}
PHP;
    $code = sprintf($code, $count);

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

    // Send each key with a small gap so the child processes them in order.
    foreach ($inputs as $input) {
        fwrite($pipes[0], $input);
        usleep(20000);
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

// Read three distinct keys in sequence inside one raw-mode loop.
// The raw-mode guard must keep the terminal raw between calls.
$output = read_keys_from_child(["\n", "\t", "\x7f"]);

echo str_contains($output, 'Enter:enter')     ? "key1-enter\n"     : $output;
echo str_contains($output, 'Tab:tab')         ? "key2-tab\n"       : $output;
echo str_contains($output, 'Backspace:backspace') ? "key3-backspace\n" : $output;
?>
--EXPECT--
key1-enter
key2-tab
key3-backspace
