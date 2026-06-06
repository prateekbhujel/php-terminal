<?php

/**
 * @generate-class-entries
 */

namespace Terminal;

enum Backend: string
{
    case Posix = 'posix';
    case Windows = 'windows';
}

enum Stream: int
{
    case Stdin = 0;
    case Stdout = 1;
    case Stderr = 2;
}

enum Key: string
{
    case Up = 'up';
    case Down = 'down';
    case Right = 'right';
    case Left = 'left';
    case Enter = 'enter';
    case Backspace = 'backspace';
    case Escape = 'escape';
    case Tab = 'tab';
    case Home = 'home';
    case End = 'end';
    case Delete = 'delete';
    case PageUp = 'pageup';
    case PageDown = 'pagedown';
}

final class Terminal
{
    public static function getBackend(): Backend {}

    public static function isTty(Stream $stream = Stream::Stdout): bool {}

    public static function supportsAnsi(Stream $stream = Stream::Stdout): bool {}

    public static function enableAnsi(Stream $stream = Stream::Stdout): bool {}

    public static function getSize(Stream $stream = Stream::Stdout): array|false {}

    public static function write(string $data, Stream $stream = Stream::Stdout): int|false {}

    public static function enableRawMode(Stream $stream = Stream::Stdin): string|false {}

    public static function restoreMode(string $mode): bool {}

    public static function readKey(?float $timeout = null, ?float $sequenceTimeout = null): Key|string|false {}

    public static function readSecret(?float $timeout = null): string|false {}
}
