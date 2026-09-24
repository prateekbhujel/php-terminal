--TEST--
readEvent preserves POSIX terminal bytes without interpreting input protocols
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
$payload = "A\xc3\xa9\r\t\x7f\x1b\x1b[A\x1bOP\x03\x1bx\x1b[1;5D\x1b[97;5u\x1b[?1u\x1b[200~paste\x1b[201~\x1b[?999z\x1b]0;title\x07\x1bPqpayload\x1b\\";
$child = <<<'PHP'
$input = fopen('php://fd/3', 'r+');
$term = Io\Terminal\Terminal::fromStreams($input);
$before = terminal_test_mode($input);
$expected = %d;
$data = '';
echo "READY\n";
while (strlen($data) < $expected) {
    $event = $term->readEvent(1.0);
    if ($event === false || $event['type'] !== 'data' || !is_string($event['data']) || $event['data'] === '' || strlen($event['data']) > 4096) {
        throw new RuntimeException('Missing or invalid data event');
    }
    $data .= $event['data'];
}
if (terminal_test_mode($input) !== $before) {
    throw new RuntimeException('Temporary raw mode was not restored');
}
echo bin2hex($data), "\n";
PHP;
$child = sprintf($child, strlen($payload));
[$output, $error, $echo, $status, $restored] = terminal_test_pty($child, $payload, true);
var_dump($output === bin2hex($payload) . "\n", $error === '', $echo === '', $status === 0, $restored);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
