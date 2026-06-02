<?php

use Terminal\Key;
use Terminal\Stream;
use Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

if (!Terminal::isTty(Stream::Stdin)) {
	fwrite(STDERR, "stdin is not a terminal\n");
	exit(1);
}

$options = [
	'Install dependencies',
	'Run tests',
	'Quit',
];

$selected = 0;
$useAnsi = Terminal::supportsAnsi();
$mode = Terminal::enableRawMode();

if (!is_string($mode)) {
	fwrite(STDERR, "could not enable raw mode\n");
	exit(1);
}

function render_prompt(array $options, int $selected, bool $useAnsi): void
{
	if ($useAnsi) {
		Terminal::write("\033[2J\033[H");
	}

	Terminal::write("Pick an action. Use arrows or j/k, then Enter.\n\n");

	foreach ($options as $index => $label) {
		$prefix = $index === $selected ? '> ' : '  ';

		if ($useAnsi && $index === $selected) {
			Terminal::write("\033[7m" . $prefix . $label . "\033[0m\n");
		} else {
			Terminal::write($prefix . $label . "\n");
		}
	}
}

try {
	render_prompt($options, $selected, $useAnsi);

	while (true) {
		$key = Terminal::readKey();

		if ($key === Key::Up || $key === 'k') {
			$selected = ($selected + count($options) - 1) % count($options);
			render_prompt($options, $selected, $useAnsi);
			continue;
		}

		if ($key === Key::Down || $key === 'j') {
			$selected = ($selected + 1) % count($options);
			render_prompt($options, $selected, $useAnsi);
			continue;
		}

		if ($key === Key::Enter) {
			break;
		}

		if ($key === Key::Escape || $key === 'q') {
			$selected = count($options) - 1;
			break;
		}
	}
} finally {
	Terminal::restoreMode($mode);
}

Terminal::write("\nSelected: " . $options[$selected] . "\n");
