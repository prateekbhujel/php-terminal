--TEST--
Session reads support descriptors beyond select FD_SETSIZE
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
$files = [];
for ($i = 0; $i < 1200; ++$i) {
    if (!is_resource($files[] = @fopen('/dev/null', 'r'))) {
        die("skip requires at least 1200 file descriptors\n");
    }
}
?>
--FILE--
<?php
require __DIR__ . '/terminal_pty.inc';
$child = <<<'PHP'
$files = [];
for ($i = 0; $i < 1100; ++$i) {
    $files[] = fopen('/dev/null', 'r');
}
$input = fopen('php://fd/3', 'r+');
$term = Io\Terminal\Terminal::fromStreams($input);
$mode = $term->enableRawMode();
echo "READY\n";
var_dump($term->readKey(1));
var_dump($term->readSecret());
PHP;
[$output, $error, $echo, $status] = terminal_test_pty($child, "xsecret\n");
echo $output;
var_dump($error === '', $echo === '', $status === 0);
?>
--EXPECT--
string(1) "x"
string(6) "secret"
bool(true)
bool(true)
bool(true)
