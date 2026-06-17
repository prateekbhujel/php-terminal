# Changelog

## Unreleased

## 0.5.0 - 2026-06-17

- Improved Windows ANSI detection so `Terminal\Terminal::supportsAnsi()` probes VT support without leaving ANSI mode enabled.
- Corrected Unix ANSI detection order, including `NO_COLOR=` handling and exact terminal capability checks.
- Added Unix F1-F12 key normalization for SS3 and CSI tilde sequences.
- Added a Windows raw-mode guard so repeated `readKey()` calls do not restore cooked mode during a raw prompt loop.
- Changed `Terminal\Terminal::getSize()` to return `cols` and `rows`.
- Changed `Terminal\Terminal::readSecret()` to accept an optional prompt, mask input, and return a string.

## 0.4.1 - 2026-06-06

- Added Windows resize-event support: `ReadConsoleInputW()` `WINDOW_BUFFER_SIZE_EVENT` now returns `Terminal\Key::Resize`.
- Documented the default 25ms POSIX sequence timeout used by `Terminal\Terminal::readKey()`.
- Documented current key-input scope, including UTF-8 code point behavior, control-byte behavior, and function-key/modifier limitations.

## 0.4.0 - 2026-06-06

- Replaced raw-mode string handles with opaque `Terminal\ModeToken` objects.
- Added `Terminal\Key::Resize` for POSIX `SIGWINCH` events surfaced by `Terminal\Terminal::readKey()`.
- Added `Terminal\Terminal::readKey()` sequence-timeout control for delayed escape and UTF-8 continuation bytes.
- Added terminal-size fallback to positive `COLUMNS` and `LINES` values.
- Hardened ANSI detection with non-empty `NO_COLOR`, `COLORTERM=truecolor`, `COLORTERM=24bit`, and `TERM_PROGRAM`.
- Hardened POSIX key-read timeout math with `CLOCK_MONOTONIC` when available.
- Hardened malformed CSI escape handling.
- Avoid redundant raw-mode enter/restore work when key or secret reads are called while the terminal is already raw.
- Documented the POSIX `TCSANOW` tradeoff for immediate raw-mode switches.
- Expanded examples, README, package metadata, and PHPT coverage for the new behavior.

## 0.3.0 - 2026-06-02

- Added `Terminal\Terminal` as the core-oriented API surface.
- Added `Terminal\Backend`, `Terminal\Stream`, and `Terminal\Key` enums.
- `Terminal\Terminal::readKey()` now returns special keys as `Terminal\Key` cases and printable input as strings.
- Removed the pre-1.0 procedural function and `TERMINAL_*` constant API.
- Updated examples, docs, and tests to use the namespaced API only.

## 0.2.0 - 2026-06-01

- Added `terminal_enable_ansi()` for ANSI/VT output enablement.
- Added `terminal_read_secret()` for hidden password/secret input.
- Hardened raw-mode behavior so prompt output keeps normal newline processing.
- Hardened Windows key reads so console mode is restored after each read.
- Added complete UTF-8 key reads on POSIX pseudo terminals.
- Hardened stream writes against zero-byte progress and partial Windows writes.
- Expanded pseudo-terminal regression coverage for common keys, UTF-8 keys, raw-mode output processing, and secret input.
- Added release documentation for Laravel Prompts/Symfony-style integration needs.

## 0.1.0 - 2026-05-24

- First public release.
- Added backend detection, TTY detection, ANSI detection, terminal sizing, direct writes, raw-mode save/restore, and key input.
- Added Unix and Windows build support.
