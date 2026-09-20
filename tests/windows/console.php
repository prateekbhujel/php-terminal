<?php

use Io\Terminal\Terminal;

$input = fopen('CONIN$', 'r+');
$output = fopen('php://memory', 'w+');
$terminal = Terminal::fromStreams($input, $output);
if (stream_isatty(STDIN)) {
    throw new RuntimeException('Test stdin must be redirected');
}
fwrite(STDOUT, "READY\n");

switch ($argv[1]) {
    case 'key':
        $key = $terminal->readKey(2.0);
        echo $key instanceof Io\Terminal\Key ? $key->name : bin2hex($key);
        break;
    case 'secret':
    case 'abort':
        try {
            echo bin2hex($terminal->readSecret('pw: '));
        } catch (RuntimeException $e) {
            echo 'cancelled';
        }
        rewind($output);
        echo '|', bin2hex(stream_get_contents($output));
        break;
    case 'raw':
        $token = $terminal->enableRawMode();
        if ($token !== $terminal->enableRawMode()) {
            throw new RuntimeException('Raw mode must be idempotent');
        }
        echo bin2hex($terminal->readKey(2.0));
        fwrite(STDOUT, "|RAW\n");
        // The parent verifies the console is still raw before releasing cleanup.
        fgets(STDIN);
        unset($terminal);
        echo 'restored';
        break;
    default:
        throw new LogicException('Unknown scenario');
}
