--TEST--
Session and legacy setTitle reject controls without output and preserve UTF-8
--EXTENSIONS--
terminal
--FILE--
<?php
use Terminal\Terminal;

// Force ANSI so a non-TTY cannot hide a broken control-character filter.
putenv('NO_COLOR');
putenv('CLICOLOR_FORCE=1');
foreach (['session', 'legacy'] as $api) {
    $stream = fopen('php://temp', 'w+');
    $session = Io\Terminal\Terminal::fromStreams(Io\Terminal\Stream::Stdin, $stream);
    $setTitle = $api === 'session'
        ? fn ($title) => $session->setTitle($title)
        : fn ($title) => Terminal::setTitle($title, $stream);

    $rejected = 0;
    foreach (array_merge(range(0, 31), [127]) as $byte) {
        ftruncate($stream, 0);
        rewind($stream);
        $result = $setTitle('Hello'.chr($byte).'World');
        rewind($stream);
        if ($result === false && stream_get_contents($stream) === '') {
            ++$rejected;
        } else {
            printf("%s failed to reject control 0x%02x without output\n", $api, $byte);
        }
    }
    echo "$api rejected controls: $rejected\n";

    foreach (['CLI Worker [active]', 'café résumé 😀'] as $title) {
        ftruncate($stream, 0);
        rewind($stream);
        var_dump($setTitle($title));
        rewind($stream);
        var_dump(stream_get_contents($stream) === "\033]0;$title\x07");
    }
    fclose($stream);
}

// Setting title on non-TTY stream without ANSI capability returns false.
putenv('NO_COLOR=1');
putenv('CLICOLOR_FORCE');
$stream = fopen('php://temp', 'w+');
var_dump(Terminal::setTitle("My Title", $stream));
var_dump(Terminal::beep($stream));
fclose($stream);

// Closed stream rejection.
$stream = fopen('php://temp', 'w+');
fclose($stream);
try {
    Terminal::setTitle("Test", $stream);
} catch (TypeError $error) {
    echo "closed stream rejected\n";
}
?>
--EXPECT--
session rejected controls: 33
bool(true)
bool(true)
bool(true)
bool(true)
legacy rejected controls: 33
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
closed stream rejected
