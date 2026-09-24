--TEST--
readEvent argument validation and non-console input behavior
--EXTENSIONS--
terminal
--FILE--
<?php
$term = Io\Terminal\Terminal::fromStreams(fopen('php://memory', 'r+'));
var_dump($term->readEvent(0.0));
foreach ([-1.0, NAN] as $timeout) {
    try {
        $term->readEvent($timeout);
    } catch (ValueError $e) {
        echo "invalid timeout\n";
    }
}
$method = new ReflectionMethod(Io\Terminal\Terminal::class, 'readEvent');
echo $method->getReturnType(), "\n";
?>
--EXPECT--
bool(false)
invalid timeout
invalid timeout
array|false
