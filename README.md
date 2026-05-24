# terminal

`terminal` is a PHP extension for basic terminal capabilities on Unix-like systems and Windows.

The first cut stays small on purpose. It exposes the pieces that are awkward to normalize in userland, especially once Windows enters the picture, without trying to become a full TUI toolkit.

## Current API

- `terminal_backend(): string`
- `terminal_is_tty(int $stream = TERMINAL_STDOUT): bool`
- `terminal_supports_ansi(int $stream = TERMINAL_STDOUT): bool`
- `terminal_get_size(int $stream = TERMINAL_STDOUT): array{columns:int, rows:int}|false`
- `terminal_write(string $data, int $stream = TERMINAL_STDOUT): int|false`
- `terminal_enable_raw_mode(int $stream = TERMINAL_STDIN): string|false`
- `terminal_restore_mode(string $mode): bool`
- `terminal_read_key(?float $timeout = null): string|false`

`terminal_write()` accepts `TERMINAL_STDOUT` and `TERMINAL_STDERR`.
`terminal_enable_raw_mode()` currently accepts `TERMINAL_STDIN` and returns an opaque mode token that should be passed back to `terminal_restore_mode()`.
`terminal_read_key()` returns printable keys as-is, named keys as strings like `up`, `down`, `left`, `right`, `enter`, `backspace`, `escape`, and `tab`, and `false` when no key is available before the timeout.

Constants:

- `TERMINAL_STDIN`
- `TERMINAL_STDOUT`
- `TERMINAL_STDERR`

## Why this exists

PHP already has useful pieces such as `stream_isatty()` and `sapi_windows_vt100_support()`, but there is still no small extension that exposes a shared terminal capability layer across Unix and Windows.

Older console-oriented extensions took different paths:

- `ncurses` and `termbox` wrap external terminal libraries
- `php-wcli` is Windows-only

This extension stays narrower:

- no ncurses dependency
- no framework coupling
- one user-facing API on both backends

## Enabling the extension

After you build and install it, enable it like any normal PHP extension:

### Unix-like systems

```ini
extension=terminal
```

### Windows

```ini
extension=php_terminal.dll
```

## Build

### Unix-like systems

```sh
phpize
./configure --enable-terminal
make
make test
```

### Windows

`config.w32` is included, and the CI workflow builds the extension with `php/php-windows-builder`.

## Example

```php
<?php

if (!extension_loaded('terminal')) {
    die("terminal is not loaded\n");
}

var_dump(terminal_backend());
var_dump(terminal_is_tty());
var_dump(terminal_supports_ansi());
var_dump(terminal_get_size());
terminal_write("hello from terminal\n");

$mode = terminal_enable_raw_mode();
if (is_string($mode)) {
    try {
        $key = terminal_read_key(0.5);
    } finally {
        terminal_restore_mode($mode);
    }
}
```

There are runnable examples in [`examples/basic.php`](./examples/basic.php) and [`examples/prompt.php`](./examples/prompt.php).
