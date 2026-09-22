<?php

use Io\Terminal\Terminal;

if ($argv[1] === 'title' || $argv[1] === 'title-resource') {
    if (!stream_isatty(STDOUT)) {
        throw new RuntimeException('Title test stdout must be a console');
    }
    $terminal = $argv[1] === 'title-resource'
        ? Terminal::fromStreams(STDIN, STDOUT)
        : Terminal::create();
    foreach (["Terminal caf\u{00e9} \u{65e5}\u{672c}\u{8a9e} \u{1f600}", "\xc3\x28", "truncated\0title", ''] as $title) {
        fwrite(STDERR, $terminal->setTitle($title) ? "true\n" : "false\n");
        if (fgets(STDIN) === false) {
            throw new RuntimeException('Missing title test acknowledgement');
        }
    }
    exit;
}

$input = fopen('CONIN$', 'r+');
$output = fopen('php://memory', 'w+');
$terminal = Terminal::fromStreams($input, $output);
if ($argv[1] !== 'key-repeat-legacy' && stream_isatty(STDIN)) {
    throw new RuntimeException('Test stdin must be redirected');
}
fwrite(STDOUT, "READY\n");

switch ($argv[1]) {
    case 'key-repeat':
    case 'key-repeat-legacy':
    case 'key-repeat-order':
        $keys = [];
        $count = $argv[1] === 'key-repeat-order' ? 6 : 3;
        for ($i = 0; $i < $count; $i++) {
            // A second session must see input left by the first one.
            $reader = $i % 2 === 0 ? $terminal : Terminal::fromStreams($input, $output);
            $key = $argv[1] === 'key-repeat-legacy' && $i === 1
                ? \Terminal\Terminal::readKey(0.0)
                : $reader->readKey($i === 0 ? 2.0 : 0.0);
            if ($key === false) {
                throw new RuntimeException('Missing repeated key');
            }
            $keys[] = $key instanceof Io\Terminal\Key ? $key->name : bin2hex($key);
        }
        if ($terminal->readKey(0.0) !== false) {
            throw new RuntimeException('Unexpected extra key');
        }
        echo implode('|', $keys);
        break;
    case 'key-secret':
        $key = $terminal->readKey(2.0);
        echo bin2hex($key), '|', bin2hex($terminal->readSecret());
        break;
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
