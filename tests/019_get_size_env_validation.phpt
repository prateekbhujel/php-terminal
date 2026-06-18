--TEST--
Terminal\Terminal::getSize rejects invalid environment fallback sizes
--EXTENSIONS--
terminal
--SKIPIF--
<?php
putenv('COLUMNS');
putenv('LINES');

if (Terminal\Terminal::getSize() !== false) {
    die('skip terminal size is available without environment fallback');
}
?>
--FILE--
<?php
$cases = [
    ['overflow', '9999999999999999999999999999999999999999', '24'],
    ['zero-cols', '0', '24'],
    ['zero-rows', '80', '0'],
    ['nondigit-cols', '80x', '24'],
    ['nondigit-rows', '80', '24x'],
    ['valid', '80', '24'],
];

foreach ($cases as [$name, $columns, $lines]) {
    putenv('COLUMNS=' . $columns);
    putenv('LINES=' . $lines);

    $size = Terminal\Terminal::getSize();
    if ($name === 'valid') {
        echo $size === ['cols' => 80, 'rows' => 24] ? "$name\n" : var_export($size, true) . "\n";
    } else {
        echo $size === false ? "$name\n" : var_export($size, true) . "\n";
    }
}

putenv('COLUMNS');
putenv('LINES');
?>
--EXPECT--
overflow
zero-cols
zero-rows
nondigit-cols
nondigit-rows
valid
