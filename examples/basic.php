<?php

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

var_dump(terminal_backend());
var_dump(terminal_is_tty());
var_dump(terminal_supports_ansi());
var_dump(terminal_get_size());
var_dump(terminal_write("hello from terminal\n"));

$mode = terminal_enable_raw_mode();
var_dump($mode === false || is_string($mode));
if (is_string($mode)) {
	var_dump(terminal_restore_mode($mode));
}
