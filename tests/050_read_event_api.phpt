--TEST--
readEvent argument validation and non-console input behavior
--EXTENSIONS--
terminal
--FILE--
<?php
$input = fopen('php://memory', 'r+');
$term = Io\Terminal\Terminal::fromStreams($input);
var_dump($term->readEvent(0.0));
foreach ([-1.0, NAN] as $timeout) {
    try {
        $term->readEvent($timeout);
    } catch (ValueError $e) {
        echo "invalid timeout\n";
    }
}
fclose($input);
try {
    $term->readEvent(0.0);
} catch (TypeError $e) {
    echo "closed-stream\n";
}
$method = new ReflectionMethod(Io\Terminal\Terminal::class, 'readEvent');
echo $method->getReturnType(), "\n";
?>
--EXPECT--
bool(false)
invalid timeout
invalid timeout
closed-stream
array|false
