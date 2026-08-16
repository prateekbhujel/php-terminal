--TEST--
Terminal\Terminal::readSecret prints a newline after the user presses Enter
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
 * Verifies that readSecret() prints a newline to stdout after the user
 * presses Enter, so subsequent output starts on a fresh line.
 *
 * The child writes a marker immediately after readSecret() returns.
 * If the newline was printed, the marker appears on its own line in the
 * captured stdout.
 */
function read_secret_check_newline(string $input): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = <<<'PHP'
echo "ready\n";
$secret = Terminal\Terminal::readSecret('pw: ');
echo "AFTER\n";
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

$output = read_secret_check_newline("test\n");

// The output should contain the newline between the masked input and AFTER.
// With PTY echo disabled (raw mode), we see: pw: ****\nAFTER\n
// The \n after **** is what readSecret() prints on Enter.
echo str_contains($output, "\nAFTER") ? "newline-on-enter\n" : $output;
?>
--EXPECT--
newline-on-enter
