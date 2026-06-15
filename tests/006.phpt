--TEST--
Terminal\Terminal::readKey reads common keys from a pseudo terminal
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
    return read_key_chunks_from_child($input === '' ? [] : [[$input, 0]], $timeout, null);
}

function read_key_chunks_from_child(array $chunks, float $timeout, ?float $sequenceTimeout): string
{
    $extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;
    $code = <<<'PHP'
echo "ready\n";
$key = Terminal\Terminal::readKey(%s, %s);
if ($key instanceof Terminal\Key) {
    echo $key->name . ':' . $key->value . "\n";
} else {
    var_dump($key);
}
PHP;
    $code = sprintf($code, var_export($timeout, true), var_export($sequenceTimeout, true));
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

    foreach ($chunks as [$input, $delayUs]) {
        if ($delayUs > 0) {
            usleep($delayUs);
        }

        if ($input !== '') {
            fwrite($pipes[0], $input);
        }
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
    'enter' => ["\n", 'Enter:enter'],
    'tab' => ["\t", 'Tab:tab'],
    'backspace' => ["\x7f", 'Backspace:backspace'],
    'escape' => ["\033", 'Escape:escape'],
    'up' => ["\033[A", 'Up:up'],
    'down' => ["\033[B", 'Down:down'],
    'right' => ["\033[C", 'Right:right'],
    'left' => ["\033[D", 'Left:left'],
    'home' => ["\033[H", 'Home:home'],
    'end' => ["\033[F", 'End:end'],
    'delete' => ["\033[3~", 'Delete:delete'],
    'pageup' => ["\033[5~", 'PageUp:pageup'],
    'pagedown' => ["\033[6~", 'PageDown:pagedown'],
    'ss3-f1' => ["\033OP", 'F1:f1'],
    'ss3-f2' => ["\033OQ", 'F2:f2'],
    'ss3-f3' => ["\033OR", 'F3:f3'],
    'ss3-f4' => ["\033OS", 'F4:f4'],
    'csi-f1' => ["\033[11~", 'F1:f1'],
    'csi-f2' => ["\033[12~", 'F2:f2'],
    'csi-f3' => ["\033[13~", 'F3:f3'],
    'csi-f4' => ["\033[14~", 'F4:f4'],
    'csi-f5' => ["\033[15~", 'F5:f5'],
    'csi-f6' => ["\033[17~", 'F6:f6'],
    'csi-f7' => ["\033[18~", 'F7:f7'],
    'csi-f8' => ["\033[19~", 'F8:f8'],
    'csi-f9' => ["\033[20~", 'F9:f9'],
    'csi-f10' => ["\033[21~", 'F10:f10'],
    'csi-f11' => ["\033[23~", 'F11:f11'],
    'csi-f12' => ["\033[24~", 'F12:f12'],
    'shift-f12' => ["\033[24;2~", 'F12:f12'],
];

foreach ($cases as $name => [$input, $expected]) {
    $output = read_key_from_child($input, 1.0);
    echo str_contains($output, $expected) ? "$name\n" : $output;
}

$timeout = read_key_from_child('', 0.05);
echo str_contains($timeout, 'bool(false)') ? "timeout\n" : $timeout;

$delayedUp = read_key_chunks_from_child([["\033", 0], ["[A", 60000]], 1.0, 0.2);
echo str_contains($delayedUp, 'Up:up') ? "delayed-up\n" : $delayedUp;
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
ss3-f1
ss3-f2
ss3-f3
ss3-f4
csi-f1
csi-f2
csi-f3
csi-f4
csi-f5
csi-f6
csi-f7
csi-f8
csi-f9
csi-f10
csi-f11
csi-f12
shift-f12
timeout
delayed-up
