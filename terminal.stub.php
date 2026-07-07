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
    case Resize = 'resize';
    case F1 = 'f1';
    case F2 = 'f2';
    case F3 = 'f3';
    case F4 = 'f4';
    case F5 = 'f5';
    case F6 = 'f6';
    case F7 = 'f7';
    case F8 = 'f8';
    case F9 = 'f9';
    case F10 = 'f10';
    case F11 = 'f11';
    case F12 = 'f12';
}

final class ModeToken
{
    private function __construct() {}
}

final class Terminal
{
    public static function getBackend(): Backend {}

    /** @param Stream|resource $stream */
    public static function isTty($stream = Stream::Stdout): bool {}

    /** @param Stream|resource $stream */
    public static function supportsAnsi($stream = Stream::Stdout): bool {}

    /** @param Stream|resource $stream */
    public static function enableAnsi($stream = Stream::Stdout): bool {}

    /** @param Stream|resource $stream */
    public static function getSize($stream = Stream::Stdout): array|false {}

    /** @param Stream|resource $stream */
    public static function write(string $data, $stream = Stream::Stdout): int|false {}

    /** @param Stream|resource $stream */
    public static function enableRawMode($stream = Stream::Stdin): ModeToken|false {}

    public static function restoreMode(ModeToken $mode): bool {}

    public static function readKey(?float $timeout = null, ?float $sequenceTimeout = null): Key|string|false {}

    public static function readSecret(string $prompt = ''): string {}
}
