--TEST--
Session and legacy setTitle reject unsafe input without output and preserve UTF-8
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
    foreach (array_merge(range(0, 31), range(127, 159)) as $byte) {
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

    $rejected = 0;
    foreach (["\xc2", "\xc3\x28", "\xc0\x80", "\xed\xa0\x80", "\xf4\x90\x80\x80"] as $title) {
        ftruncate($stream, 0);
        rewind($stream);
        $result = $setTitle($title);
        rewind($stream);
        if ($result === false && stream_get_contents($stream) === '') {
            ++$rejected;
        } else {
            echo "$api accepted malformed UTF-8\n";
        }
    }
    echo "$api rejected malformed UTF-8: $rejected\n";

    $rejected = 0;
    foreach (range(0x80, 0x9f) as $byte) {
        ftruncate($stream, 0);
        rewind($stream);
        $result = $setTitle("\xc2".chr($byte));
        rewind($stream);
        if ($result === false && stream_get_contents($stream) === '') {
            ++$rejected;
        } else {
            echo "$api accepted C1 character\n";
        }
    }
    echo "$api rejected C1 characters: $rejected\n";

    foreach (['CLI Worker [active]', "U+00A0 \xc2\xa0", '日本語', 'café résumé 😀'] as $title) {
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
session rejected controls: 65
session rejected malformed UTF-8: 5
session rejected C1 characters: 32
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
legacy rejected controls: 65
legacy rejected malformed UTF-8: 5
legacy rejected C1 characters: 32
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
closed stream rejected
