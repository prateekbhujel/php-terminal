--TEST--
readEvent consumes PHP-buffered bytes and preserves an outer raw mode
--EXTENSIONS--
terminal
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows' || !function_exists('proc_open')) {
    die("skip requires POSIX pseudo terminals\n");
}
$p = @proc_open([PHP_BINARY, '-r', 'exit(0);'], [0 => ['pty'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
if (!is_resource($p)) {
    die("skip pseudo terminal is unavailable\n");
}
foreach ($pipes as $pipe) fclose($pipe);
proc_close($p);
?>
--FILE--
<?php
require __DIR__ . '/terminal_pty.inc';
$child = <<<'PHP'
$input = fopen('php://fd/3', 'r+');
$term = Io\Terminal\Terminal::fromStreams($input);
$before = terminal_test_mode($input);
$token = $term->enableRawMode();
$raw = terminal_test_mode($input);
var_dump($term->readEvent(0.0));
echo "READY\n";
$first = fread($input, 1);
$buffered = stream_get_meta_data($input)['unread_bytes'] > 0;
$data = '';
while (strlen($data) < 8) {
    $event = $term->readEvent(0.1);
    if ($event === false || $event['type'] !== 'data') {
        throw new RuntimeException('Missing data event');
    }
    $data .= $event['data'];
}
var_dump($first, $buffered, bin2hex($data), $term->readEvent(0.0));
echo terminal_test_mode($input) === $raw ? "still-raw\n" : "wrong-mode\n";
unset($term);
echo terminal_test_mode($input) === $before ? "restored\n" : "wrong-mode\n";
PHP;
[$output, $error, $echo, $status] = terminal_test_pty($child, "x\x1b[1;5D\xc3\xa9", true);
echo $output;
var_dump($error === '', $echo === '', $status === 0);
?>
--EXPECT--
bool(false)
string(1) "x"
bool(true)
string(16) "1b5b313b3544c3a9"
bool(false)
still-raw
restored
bool(true)
bool(true)
bool(true)
