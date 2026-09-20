--TEST--
Session reads fail predictably for non-terminal and closed input streams
--EXTENSIONS--
terminal
--FILE--
<?php
foreach ([fopen('php://memory', 'r+'), tmpfile()] as $input) {
    $term = Io\Terminal\Terminal::fromStreams($input);
    var_dump($term->readKey(0));
    try {
        $term->readSecret();
    } catch (RuntimeException $e) {
        echo get_class($e), "\n";
    }
    fclose($input);
    try {
        $term->readKey(0);
    } catch (TypeError $e) {
        echo "closed-stream\n";
    }
}
?>
--EXPECT--
bool(false)
RuntimeException
closed-stream
bool(false)
RuntimeException
closed-stream
