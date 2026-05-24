<?php

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

if (!terminal_is_tty(TERMINAL_STDIN)) {
	fwrite(STDERR, "stdin is not a terminal\n");
	exit(1);
}

$options = [
	'Install dependencies',
	'Run tests',
	'Quit',
];

$selected = 0;
$useAnsi = terminal_supports_ansi();
$mode = terminal_enable_raw_mode();

if (!is_string($mode)) {
	fwrite(STDERR, "could not enable raw mode\n");
	exit(1);
}

function render_prompt(array $options, int $selected, bool $useAnsi): void
{
	if ($useAnsi) {
		terminal_write("\033[2J\033[H");
	}

	terminal_write("Pick an action. Use arrows or j/k, then Enter.\n\n");

	foreach ($options as $index => $label) {
		$prefix = $index === $selected ? '> ' : '  ';

		if ($useAnsi && $index === $selected) {
			terminal_write("\033[7m" . $prefix . $label . "\033[0m\n");
		} else {
			terminal_write($prefix . $label . "\n");
		}
	}
}

try {
	render_prompt($options, $selected, $useAnsi);

	while (true) {
		$key = terminal_read_key();

		if ($key === 'up' || $key === 'k') {
			$selected = ($selected + count($options) - 1) % count($options);
			render_prompt($options, $selected, $useAnsi);
			continue;
		}

		if ($key === 'down' || $key === 'j') {
			$selected = ($selected + 1) % count($options);
			render_prompt($options, $selected, $useAnsi);
			continue;
		}

		if ($key === 'enter') {
			break;
		}

		if ($key === 'escape' || $key === 'q') {
			$selected = count($options) - 1;
			break;
		}
	}
} finally {
	terminal_restore_mode($mode);
}

terminal_write("\nSelected: " . $options[$selected] . "\n");
