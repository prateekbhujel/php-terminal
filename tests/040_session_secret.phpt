--TEST--
Session secret input uses a custom TTY, stays silent, and restores mode on submit and abort
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
require __DIR__ . '/terminal_pty.inc';

$child = <<<'PHP'
use Io\Terminal\Terminal;
$input = fopen('php://fd/3', 'r+');
$output = fopen('php://memory', 'w+');
$term = Terminal::fromStreams($input, $output);
$before = terminal_test_mode($input);
echo "READY\n";
try {
    $value = $term->readSecret('');
    echo 'secret:' . bin2hex($value) . "\n";
} catch (RuntimeException $e) {
    echo get_class($e) . "\n";
}
echo terminal_test_mode($input) === $before ? "restored\n" : "wrong-mode\n";
rewind($output);
echo 'output:' . bin2hex(stream_get_contents($output)) . "\n";
PHP;
foreach (["ab\x7fcaf\xc3\xa9\n", "\n", "bad\x03", "bad\x04", "bad\x1b"] as $input) {
    [$output, $error, $echo, $status] = terminal_test_pty($child, $input);
    echo $output;
    var_dump($error === '', $echo === '', $status === 0);
}
// An explicit prompt goes only through the configured PHP output stream.
[$output, $error, $echo, $status] = terminal_test_pty(str_replace("readSecret('')", "readSecret('pw: ')", $child), "x\n");
echo $output;
var_dump($error === '', $echo === '', $status === 0);
?>
--EXPECT--
secret:61636166c3a9
restored
output:
bool(true)
bool(true)
bool(true)
secret:
restored
output:
bool(true)
bool(true)
bool(true)
RuntimeException
restored
output:
bool(true)
bool(true)
bool(true)
RuntimeException
restored
output:
bool(true)
bool(true)
bool(true)
RuntimeException
restored
output:
bool(true)
bool(true)
bool(true)
secret:78
restored
output:70773a20
bool(true)
bool(true)
bool(true)
