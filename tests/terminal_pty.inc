<?php

function terminal_test_read_pty($stream): string
{
    // Linux reports EIO when the last slave closes; other read errors remain visible.
    set_error_handler(static fn ($severity, $message) => str_contains($message, 'errno=5 '));
    try {
        return (string) stream_get_contents($stream);
    } finally {
        restore_error_handler();
    }
}

function terminal_test_mode($input): string
{
    $process = proc_open(['stty', '-g'], [0 => $input, 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    $mode = trim(stream_get_contents($pipes[1]));
    $error = stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    if (proc_close($process) !== 0 || $error !== '') {
        throw new RuntimeException('Cannot inspect terminal mode: ' . $error);
    }
    // Darwin changes PENDIN when input is retyped; it is not a raw-mode setting.
    if (PHP_OS_FAMILY === 'Darwin') {
        $mode = preg_replace_callback('/:lflag=([0-9a-f]+):/', static fn ($m) => ':lflag=' . dechex(hexdec($m[1]) & ~0x20000000) . ':', $mode);
    }
    return $mode;
}

/** Run with a custom TTY at fd 3 and unrelated, non-terminal process stdin. */
function terminal_test_pty(string $code, ?string $input, bool $checkRestoration = false): array
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = 'require ' . var_export(__FILE__, true) . '; fgets(STDIN);' . $code;
    $process = proc_open([PHP_BINARY, '-n', '-d', 'extension=' . $extension, '-r', $code], [
        0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w'], 3 => ['pty'],
    ], $pipes);
    $before = terminal_test_mode($pipes[3]);
    fwrite($pipes[0], "go\n");
    foreach ([1, 2, 3] as $fd) {
        stream_set_blocking($pipes[$fd], false);
    }
    $output = $error = $echo = '';
    $sent = false;
    $deadline = microtime(true) + 5;
    do {
        $output .= stream_get_contents($pipes[1]);
        $error .= stream_get_contents($pipes[2]);
        $echo .= terminal_test_read_pty($pipes[3]);
        if (!$sent && $input !== null && str_contains($output, "READY\n") && terminal_test_mode($pipes[3]) !== $before) {
            fwrite($pipes[3], $input);
            $sent = true;
        }
        $status = proc_get_status($process);
        if (!$status['running']) {
            break;
        }
        usleep(10000);
    } while (microtime(true) < $deadline);
    if ($status['running']) {
        proc_terminate($process, 9);
        $error .= 'child timed out';
    }
    $output .= stream_get_contents($pipes[1]);
    $error .= stream_get_contents($pipes[2]);
    $echo .= terminal_test_read_pty($pipes[3]);
    $restored = !$checkRestoration || $before === terminal_test_mode($pipes[3]);
    foreach ($pipes as $pipe) {
        fclose($pipe);
    }
    proc_close($process);
    return [str_replace("READY\n", '', $output), $error, $echo, $status['exitcode'], $restored];
}
