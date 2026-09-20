<?php

use Io\Terminal\Key;
use Io\Terminal\ModeToken;
use Io\Terminal\Terminal;

if (!extension_loaded('terminal')) {
	fwrite(STDERR, "terminal extension is not loaded\n");
	exit(1);
}

if (!stream_isatty(STDIN)) {
	fwrite(STDERR, "stdin is not a terminal\n");
	exit(1);
}

$options = [
	'Install dependencies',
	'Run tests',
	'Quit',
];

$terminal = Terminal::create();
$selected = 0;
$useAnsi = $terminal->enableAnsi();
$mode = $terminal->enableRawMode();

if (!$mode instanceof ModeToken) {
	fwrite(STDERR, "could not enable raw mode\n");
	exit(1);
}

function render_prompt(Terminal $terminal, array $options, int $selected, bool $useAnsi): void
{
	if ($useAnsi) {
		$terminal->write("\033[2J\033[H");
	}

	$terminal->write("Pick an action. Use arrows or j/k, then Enter.\n\n");

	foreach ($options as $index => $label) {
		$prefix = $index === $selected ? '> ' : '  ';

		if ($useAnsi && $index === $selected) {
			$terminal->write("\033[7m" . $prefix . $label . "\033[0m\n");
		} else {
			$terminal->write($prefix . $label . "\n");
		}
	}
}

try {
	render_prompt($terminal, $options, $selected, $useAnsi);

	while (true) {
		$key = $terminal->readKey();
		if ($key === false) {
			throw new RuntimeException('Terminal input is unavailable.');
		}

		if ($key === Key::Resize) {
			render_prompt($terminal, $options, $selected, $useAnsi);
			continue;
		}

		if ($key === Key::Up || $key === 'k') {
			$selected = ($selected + count($options) - 1) % count($options);
			render_prompt($terminal, $options, $selected, $useAnsi);
			continue;
		}

		if ($key === Key::Down || $key === 'j') {
			$selected = ($selected + 1) % count($options);
			render_prompt($terminal, $options, $selected, $useAnsi);
			continue;
		}

		if ($key === Key::Enter) {
			break;
		}

		if ($key === Key::Escape || $key === 'q' || $key === "\x03" || $key === "\x04") {
			$selected = count($options) - 1;
			break;
		}
	}
} finally {
	if (!$terminal->restoreMode()) {
		throw new RuntimeException('Could not restore terminal mode.');
	}
}

$terminal->write("\nSelected: " . $options[$selected] . "\n");
