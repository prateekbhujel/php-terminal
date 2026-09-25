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
    case 'event-key':
        $event = $terminal->readEvent(2.0);
        if ($event === false) {
            throw new RuntimeException('Missing console key event');
        }
        echo implode('|', [
            $event['type'], $event['key']?->name ?? '-', $event['text'] === null ? '-' : bin2hex($event['text']),
            (int) $event['keyDown'], $event['repeatCount'], $event['virtualKeyCode'],
            $event['virtualScanCode'], $event['unicodeCodeUnit'], $event['controlKeyState'],
            (int) $event['ctrl'], (int) $event['alt'], (int) $event['shift'],
        ]);
        break;
    case 'key-surrogate-split':
        echo "READY1\n";
        $first = $terminal->readKey(0.2);
        echo $first === false ? "false\n" : bin2hex($first) . "\n";
        echo "READY2\n";
        $second = $terminal->readKey(2.0);
        echo $second === false ? 'false' : bin2hex($second);
        break;
    case 'event-surrogates':
        $events = [$terminal->readEvent(2.0), $terminal->readEvent(2.0)];
        foreach ($events as $event) {
            if ($event === false || $event['type'] !== 'key') {
                throw new RuntimeException('Missing UTF-16 key record');
            }
        }
        echo implode('|', array_map(static fn ($event) => $event['unicodeCodeUnit'] . ':' . ($event['text'] === null ? '-' : bin2hex($event['text'])), $events));
        break;
    case 'event-pending':
        $key = $terminal->readKey(2.0);
        $other = Terminal::fromStreams($input, $output);
        $event = $other->readEvent(0.0);
        if ($event === false || $event['type'] !== 'key') {
            throw new RuntimeException('Missing pending repetitions');
        }
        echo bin2hex($key), '|', $event['repeatCount'], '|', bin2hex($event['text']), '|', $terminal->readEvent(0.0) === false ? 'false' : 'extra';
        break;
    case 'event-resize':
        $event = $terminal->readEvent(2.0);
        echo $event['type'], '|', $event['bufferCols'], '|', $event['bufferRows'];
        break;
    case 'event-mouse':
        $event = $terminal->readEvent(2.0);
        echo implode('|', [$event['type'], $event['x'], $event['y'], $event['buttonState'], $event['controlKeyState'], $event['eventFlags']]);
        break;
    case 'event-focus':
        $event = $terminal->readEvent(2.0);
        echo $event['type'], '|', (int) $event['focused'];
        break;
    case 'event-menu':
        $event = $terminal->readEvent(2.0);
        echo $event['type'], '|', $event['commandId'];
        break;
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
    case 'raw-overlap':
        $a = Terminal::fromStreams($input, $output);
        $b = Terminal::fromStreams($input, $output);
        $a->enableRawMode();
        $b->enableRawMode();
        echo "OVERLAP\n";
        fgets(STDIN);
        echo $a->restoreMode() ? "true|FIRST\n" : "false|FIRST\n";
        fgets(STDIN);
        echo $b->restoreMode() ? 'true' : 'false';
        break;
    case 'raw':
    case 'event-raw':
        $token = $terminal->enableRawMode();
        if ($token !== $terminal->enableRawMode()) {
            throw new RuntimeException('Raw mode must be idempotent');
        }
        echo $argv[1] === 'event-raw'
            ? bin2hex($terminal->readEvent(2.0)['text'])
            : bin2hex($terminal->readKey(2.0));
        if ($argv[1] === 'event-raw' && $terminal->readEvent(0.0) !== false) {
            throw new RuntimeException('Expected empty poll');
        }
        fwrite(STDOUT, "|RAW\n");
        // The parent verifies the console is still raw before releasing cleanup.
        fgets(STDIN);
        if ($argv[1] === 'event-raw') {
            $resize = $terminal->readEvent(0.0);
            if ($resize === false || $resize['type'] !== 'resize') {
                throw new RuntimeException('Resize was not queued between reads');
            }
            echo $resize['type'], '|', $resize['bufferCols'], '|', $resize['bufferRows'], '|';
        }
        unset($terminal);
        echo 'restored';
        break;
    default:
        throw new LogicException('Unknown scenario');
}
