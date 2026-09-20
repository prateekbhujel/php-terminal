--TEST--
Session construction rejects reinitialization, invalid stream objects, cloning and serialization
--EXTENSIONS--
terminal
--FILE--
<?php
use Io\Terminal\Terminal;
$input = fopen('php://memory', 'r+');
$term = Terminal::fromStreams($input);
foreach ([
    fn () => $term->__construct($term),
    fn () => Terminal::fromStreams($term),
    fn () => Terminal::fromStreams(),
    fn () => clone $term,
    fn () => serialize($term),
] as $action) {
    try {
        $action();
        echo "unexpected success\n";
    } catch (Throwable $e) {
        echo get_class($e), "\n";
    }
}
var_dump($term->getInputStream() === $input);
var_dump((new ReflectionMethod(Terminal::class, 'fromStreams'))->getNumberOfRequiredParameters());
?>
--EXPECT--
Error
TypeError
ArgumentCountError
Error
Exception
bool(true)
int(1)
