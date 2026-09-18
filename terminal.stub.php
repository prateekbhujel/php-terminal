<?php

/**
 * @generate-function-entries
 * @generate-class-entries
 */

namespace Io\Terminal
{
    /**
     * @alias Terminal\Backend
     */
    enum Backend
    {
        case Posix;
        case Windows;
    }

    /**
     * @alias Terminal\Stream
     */
    enum Stream
    {
        case Stdin;
        case Stdout;
        case Stderr;
    }

    /**
     * @alias Terminal\ColorDepth
     */
    enum ColorDepth
    {
        case None;
        case Standard;
        case Extended;
        case TrueColor;

        public function bits(): int {}
    }

    /**
     * @alias Terminal\Key
     */
    enum Key
    {
        case Up;
        case Down;
        case Right;
        case Left;
        case Enter;
        case Backspace;
        case Escape;
        case Tab;
        case Home;
        case End;
        case Delete;
        case PageUp;
        case PageDown;
        case Resize;
        case F1;
        case F2;
        case F3;
        case F4;
        case F5;
        case F6;
        case F7;
        case F8;
        case F9;
        case F10;
        case F11;
        case F12;
    }

    /**
     * @alias Terminal\ModeToken
     */
    final class ModeToken
    {
        private function __construct() {}
    }

    function get_backend(): Backend {}

    function is_tty(mixed $stream = UNKNOWN): bool {}

    function supports_ansi(mixed $stream = UNKNOWN): bool {}

    function enable_ansi(mixed $stream = UNKNOWN): bool {}

    function get_size(mixed $stream = UNKNOWN): array|false {}

    function get_width(mixed $stream = UNKNOWN): int|false {}

    function get_height(mixed $stream = UNKNOWN): int|false {}

    function get_color_depth(mixed $stream = UNKNOWN): ColorDepth {}

    function supports_color(ColorDepth $depth = UNKNOWN, mixed $stream = UNKNOWN): bool {}

    function supports_true_color(mixed $stream = UNKNOWN): bool {}

    function set_title(string $title, mixed $stream = UNKNOWN): bool {}

    function beep(mixed $stream = UNKNOWN): bool {}

    function write(string $data, mixed $stream = UNKNOWN): int|false {}

    function enable_raw_mode(mixed $stream = UNKNOWN): ModeToken|false {}

    function restore_mode(ModeToken $mode): bool {}

    function read_key(?float $timeout = null, ?float $sequenceTimeout = null, mixed $stream = UNKNOWN): Key|string|false {}

    function read_secret(string $prompt = "", mixed $stream = UNKNOWN): string {}

    class Terminal
    {
        public function __construct(mixed $stream = UNKNOWN) {}

        public static function stdin(): Terminal {}

        public static function stdout(): Terminal {}

        public static function stderr(): Terminal {}

        public static function getBackend(): Backend {}

        public function getStream(): mixed {}

        public function isTty(): bool {}

        public function supportsAnsi(): bool {}

        public function enableAnsi(): bool {}

        public function getSize(): array|false {}

        public function getWidth(): int|false {}

        public function getHeight(): int|false {}

        public function getColorDepth(): ColorDepth {}

        public function supportsColor(ColorDepth $depth = UNKNOWN): bool {}

        public function supportsTrueColor(): bool {}

        public function setTitle(string $title): bool {}

        public function beep(): bool {}

        public function write(string $data): int|false {}

        public function enableRawMode(): ModeToken|false {}

        public function restoreMode(?ModeToken $mode = null): bool {}

        public function readKey(?float $timeout = null, ?float $sequenceTimeout = null): Key|string|false {}

        public function readSecret(string $prompt = ""): string {}
    }
}

namespace Terminal
{
    final class Terminal
    {
        public static function getBackend(): \Io\Terminal\Backend {}

        public static function isTty(mixed $stream = UNKNOWN): bool {}

        public static function supportsAnsi(mixed $stream = UNKNOWN): bool {}

        public static function enableAnsi(mixed $stream = UNKNOWN): bool {}

        public static function getSize(mixed $stream = UNKNOWN): array|false {}

        public static function getWidth(mixed $stream = UNKNOWN): int|false {}

        public static function getHeight(mixed $stream = UNKNOWN): int|false {}

        public static function getColorDepth(mixed $stream = UNKNOWN): \Io\Terminal\ColorDepth {}

        public static function supportsColor(\Io\Terminal\ColorDepth $depth = UNKNOWN, mixed $stream = UNKNOWN): bool {}

        public static function supportsTrueColor(mixed $stream = UNKNOWN): bool {}

        public static function setTitle(string $title, mixed $stream = UNKNOWN): bool {}

        public static function beep(mixed $stream = UNKNOWN): bool {}

        public static function write(string $data, mixed $stream = UNKNOWN): int|false {}

        public static function enableRawMode(mixed $stream = UNKNOWN): \Io\Terminal\ModeToken|false {}

        public static function restoreMode(\Io\Terminal\ModeToken $mode): bool {}

        public static function readKey(?float $timeout = null, ?float $sequenceTimeout = null): \Io\Terminal\Key|string|false {}

        public static function readSecret(string $prompt = ""): string {}
    }
}
