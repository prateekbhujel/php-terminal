--TEST--
Secret prompt callbacks may close input or throw without stale native reads
--EXTENSIONS--
terminal
--FILE--
<?php
class PromptStream {
    public $context;
    public static $input;
    public static bool $throws = false;
    public function stream_open($path, $mode, $options, &$opened_path) { return true; }
    public function stream_cast($cast_as) { return false; }
    public function stream_write($data) {
        if (self::$throws) {
            throw new LogicException('prompt failed');
        }
        fclose(self::$input);
        return strlen($data);
    }
}
stream_wrapper_register('prompt', PromptStream::class);
foreach ([false, true] as $throws) {
    PromptStream::$throws = $throws;
    PromptStream::$input = tmpfile();
    $term = Io\Terminal\Terminal::fromStreams(PromptStream::$input, fopen('prompt://test', 'w'));
    try {
        $term->readSecret('Secret: ');
    } catch (Throwable $e) {
        echo get_class($e), "\n";
    }
}
?>
--EXPECT--
TypeError
LogicException
