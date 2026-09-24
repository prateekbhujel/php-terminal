# Integration guide

`php-terminal` is optional infrastructure. Libraries should keep their existing
userland behavior when the extension is absent or outside the API range they
support.

## Detect the supported API, not only the extension name

If an integration is written against the 1.x contract, use a bounded version
check and verify the class without invoking an autoloader:

```php
function hasTerminalV1(): bool
{
    if (!extension_loaded('terminal')) {
        return false;
    }

    $version = phpversion('terminal');

    return false !== $version
        && version_compare($version, '1.0.0', '>=')
        && version_compare($version, '2.0.0', '<')
        && class_exists(\Io\Terminal\Terminal::class, false);
}
```

The upper bound matters because 2.x is allowed to change incompatible API. The
class check also avoids assuming that any unrelated extension using the same
extension name provides this API.

## Keep a userland fallback

Native support should normally be an optimization or portability improvement,
not a hard dependency:

```php
if (hasTerminalV1()) {
    $size = \Io\Terminal\Terminal::create()->getSize();

    if (false !== $size) {
        return [$size->cols, $size->rows];
    }
}

// Existing stty, environment or platform fallback.
```

Do not turn an optional extension failure into a fatal error when the library
already has a working fallback.

## Preserve the caller's streams

For prompts and testable console abstractions, use the stream already owned by
the library:

```php
$terminal = \Io\Terminal\Terminal::fromStreams($inputStream, $outputStream);
```

Hidden input and raw mode act on input. Size, ANSI/color detection, writes,
title and bell act on output. Avoid silently replacing redirected/custom streams
with process-global `STDIN` or `STDOUT`.

## Hidden input

`readSecret()` is silent. The surrounding library remains responsible for the
prompt, masks if it wants them, trailing newline, validation and fallback policy.

Cancellation and operational failures are `RuntimeException`. If an existing
prompt abstraction supports "show input visibly if hiding fails", translate the
native exception into that existing behavior rather than leaking a new stack
trace to users.

## TUI loops

Keep one session alive for raw input loops instead of constructing a session per
key. Raw-mode ownership belongs to that session, and explicit restore belongs in
a `finally` block.

For a prompt that only needs common keys, use `readKey()`. A TUI parser that
needs original escape sequences or Windows modifier/repeat/release records can
use `readEvent()` starting with extension 1.1.0. The `hasTerminalV1()` example
above also accepts 1.0.x. Check for version 1.1.0 or newer within that 1.x
range before calling the new method.

On POSIX, append each `data` chunk to the parser's input buffer. Chunks are
arbitrary byte boundaries, so do not interpret a lone high byte as Alt/Meta or
assume one chunk is one key or complete UTF-8 character. The extension leaves
`SIGWINCH` to the application's signal watcher. On Windows, dispatch by the
event's `type` and use the original virtual key, UTF-16 unit and control-state
fields when mapping input. `readEvent()` does not turn native key records into
terminal escape sequences.

Use `getSize()` on resize paths rather than spawning `stty size` when the
extension is available. On Windows, native raw mode and console key events avoid
the Unix-only `stty` dependency.

## Testing an integration

Cover both branches:

- extension absent or unsupported version
- supported 1.x extension
- native input cancellation/failure
- hidden input does not echo
- configured streams are respected
- terminal mode is restored
- dimensions come from the intended output terminal

Real PTY/native console tests are more useful than mocks for behavior that
depends on terminal mode.
