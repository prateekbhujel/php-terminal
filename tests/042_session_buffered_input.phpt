--TEST--
Session key and secret reads consume PHP-buffered input before native bytes
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
$input = fopen('php://fd/3', 'r+');
$term = Io\Terminal\Terminal::fromStreams($input);
$token = $term->enableRawMode();
echo "READY\n";
// fread prefetches the rest into PHP's buffer, including the escape and UTF-8 sequence.
var_dump(fread($input, 1));
var_dump(stream_get_meta_data($input)['unread_bytes'] > 0);
var_dump($term->readKey(0.1) === Io\Terminal\Key::Up);
var_dump($term->readSecret());
PHP;
[$output, $error, $echo, $status] = terminal_test_pty($child, "x\x1b[Acaf\xc3\xa9\n");
echo $output;
var_dump($error === '', $echo === '', $status === 0);
?>
--EXPECT--
string(1) "x"
bool(true)
bool(true)
string(5) "café"
bool(true)
bool(true)
bool(true)
