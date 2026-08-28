--TEST--
Terminal\Terminal::readSecret prints a newline and throws on Escape abort
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
 * Verifies that readSecret() prints a newline to stdout and throws an
 * error when the user presses Escape (0x1b) to abort.
 */
function read_secret_abort(string $input): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = <<<'PHP'
echo "ready\n";
try {
    $secret = Terminal\Terminal::readSecret('pw: ');
    echo "SECRET:" . $secret . "\n";
} catch (\Error $e) {
    echo "ERROR:" . $e->getMessage() . "\n";
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

    proc_close($process);

    return $output . $error;
}

// Send Escape (0x1b) to abort the secret input.
$output = read_secret_abort("\x1b");

echo str_contains($output, 'ERROR:Unable to read secret from terminal') ? "abort-throws\n" : $output;
echo str_contains($output, "\nERROR:") ? "newline-before-error\n" : "no-newline\n";
?>
--EXPECT--
abort-throws
newline-before-error
