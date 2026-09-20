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

    /**
     * @alias Terminal\TerminalSize
     */
    final class TerminalSize
    {
        public readonly int $cols;
        public readonly int $rows;
        public readonly int $width;
        public readonly int $height;

        public function __construct(int $cols, int $rows) {}

        public function toArray(): array {}
    }

    class Terminal
    {
        public function __construct(mixed $input = UNKNOWN, mixed $output = UNKNOWN) {}

        public static function create(): Terminal {}

        public static function open(): Terminal {}

        public static function fromStreams(mixed $input, mixed $output = UNKNOWN): Terminal {}

        public static function fromStream(mixed $stream): Terminal {}

        public static function getBackend(): Backend {}

        public function getInputStream(): mixed {}

        public function getOutputStream(): mixed {}

        public function getStream(): mixed {}

        public function isTty(): bool {}

        public function supportsAnsi(): bool {}

        public function enableAnsi(): bool {}

        public function getSize(): TerminalSize|false {}

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
