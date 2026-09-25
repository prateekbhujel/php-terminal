<?php

require dirname(__DIR__) . '/terminal_pty.inc';

if (!extension_loaded('parallel')) {
    throw new RuntimeException('parallel extension is required');
}
if (!defined('PHP_ZTS') || !PHP_ZTS) {
    throw new RuntimeException('ZTS PHP is required');
}

$extension = dirname(__DIR__, 2) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;

$child = <<<'PHP'
pcntl_async_signals(true);

$count = 0;
pcntl_signal(SIGWINCH, function () use (&$count): void {
    $count++;
});

posix_kill(getmypid(), SIGWINCH);
usleep(100000);
echo "BASELINE|count={$count}\n";
fflush(STDOUT);
$count = 0;

$ready = parallel\Channel::make('terminal-zts-ready', parallel\Channel::Infinite);
$startA = parallel\Channel::make('terminal-zts-start-a');
$startB = parallel\Channel::make('terminal-zts-start-b');

$runtimeA = new parallel\Runtime();
$runtimeB = new parallel\Runtime();

$futureA = $runtimeA->run(static function ($ready, $start): string {
    $input = fopen('php://fd/3', 'r+');
    $terminal = Io\Terminal\Terminal::fromStreams($input);
    $ready->send('A');
    $start->recv();
    $value = $terminal->readKey(0.40);

    return $value === false
        ? 'false'
        : ($value instanceof Io\Terminal\Key ? $value->name : bin2hex($value));
}, [$ready, $startA]);

if ($ready->recv() !== 'A') {
    throw new RuntimeException('A did not start');
}
$startA->send(true);
usleep(150000);

$futureB = $runtimeB->run(static function ($ready, $start): string {
    $input = fopen('php://fd/4', 'r+');
    $terminal = Io\Terminal\Terminal::fromStreams($input);
    $ready->send('B');
    $start->recv();
    $value = $terminal->readKey(0.80);

    return $value === false
        ? 'false'
        : ($value instanceof Io\Terminal\Key ? $value->name : bin2hex($value));
}, [$ready, $startB]);

if ($ready->recv() !== 'B') {
    throw new RuntimeException('B did not start');
}
$startB->send(true);

$a = $futureA->value();
$b = $futureB->value();

echo "READ|A={$a}|B={$b}\n";
fflush(STDOUT);

posix_kill(getmypid(), SIGWINCH);
usleep(100000);

echo "SIGNAL|count={$count}\n";
PHP;

$process = proc_open(
    [PHP_BINARY, '-d', 'extension=' . $extension, '-r', $child],
    [
        0 => ['pipe', 'r'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
        3 => ['pty'],
        4 => ['pty'],
    ],
    $pipes
);

if (!is_resource($process)) {
    throw new RuntimeException('Could not start ZTS child');
}

$beforeA = terminal_test_mode($pipes[3]);
$beforeB = terminal_test_mode($pipes[4]);

stream_set_blocking($pipes[1], false);
stream_set_blocking($pipes[2], false);

$output = $error = '';
$deadline = microtime(true) + 8;

do {
    $output .= stream_get_contents($pipes[1]);
    $error .= stream_get_contents($pipes[2]);

    $status = proc_get_status($process);
    if (!$status['running']) {
        break;
    }

    usleep(10000);
} while (microtime(true) < $deadline);

if ($status['running']) {
    proc_terminate($process, 9);
    throw new RuntimeException('ZTS child timed out');
}

$output .= stream_get_contents($pipes[1]);
$error .= stream_get_contents($pipes[2]);

$restoredA = $beforeA === terminal_test_mode($pipes[3]);
$restoredB = $beforeB === terminal_test_mode($pipes[4]);

foreach ($pipes as $pipe) {
    fclose($pipe);
}
proc_close($process);

if ($error !== '') {
    throw new RuntimeException($error);
}
if (!$restoredA || !$restoredB) {
    throw new RuntimeException('PTY mode was not restored');
}
if (!str_contains($output, "BASELINE|count=1\n")) {
    throw new RuntimeException('Baseline SIGWINCH handler did not run: ' . $output);
}
if (!str_contains($output, "READ|A=false|B=false\n")) {
    throw new RuntimeException('Concurrent reads did not time out normally: ' . $output);
}
if (!str_contains($output, "SIGNAL|count=1\n")) {
    throw new RuntimeException('SIGWINCH handler was not restored after concurrent reads: ' . $output);
}

echo "concurrent SIGWINCH restoration: passed\n";
