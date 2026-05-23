<?php

/**
 * @generate-class-entries
 */

function terminal_backend(): string {}

function terminal_is_tty(int $stream = TERMINAL_STDOUT): bool {}

function terminal_supports_ansi(int $stream = TERMINAL_STDOUT): bool {}

function terminal_get_size(int $stream = TERMINAL_STDOUT): array|false {}

function terminal_write(string $data, int $stream = TERMINAL_STDOUT): int|false {}

function terminal_enable_raw_mode(int $stream = TERMINAL_STDIN): string|false {}

function terminal_restore_mode(string $mode): bool {}
