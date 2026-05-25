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

## Trying 0.1.0

The `v0.1.0` alpha release is available at:

https://github.com/prateekbhujel/php-terminal/releases/tag/v0.1.0

Windows builds are attached for PHP 8.2-8.5, x64, TS/NTS. Pick the zip that matches your PHP version and thread-safety mode, copy `php_terminal.dll` into your PHP extension directory, and enable it with:

```ini
extension=php_terminal.dll
```

### XAMPP on Windows

XAMPP works as long as the DLL matches the PHP build bundled with your XAMPP install.

Check the PHP version, thread-safety mode, architecture, and compiler:

```bat
C:\xampp\php\php.exe -i | findstr /C:"PHP Version" /C:"Thread Safety" /C:"Architecture" /C:"Compiler"
```

Download the matching zip from the release page. For example:

- PHP 8.2, thread safety disabled: `php_terminal-v0.1.0-8.2-nts-vs16-x86_64.zip`
- PHP 8.2, thread safety enabled: `php_terminal-v0.1.0-8.2-ts-vs16-x86_64.zip`
- PHP 8.4, thread safety disabled: `php_terminal-v0.1.0-8.4-nts-vs17-x86_64.zip`

Copy `php_terminal.dll` into:

```text
C:\xampp\php\ext
```

Then edit:

```text
C:\xampp\php\php.ini
```

Add:

```ini
extension=php_terminal.dll
```

Test it with XAMPP's CLI PHP:

```bat
C:\xampp\php\php.exe -m | findstr terminal
C:\xampp\php\php.exe examples\prompt.php
```

On Unix-like systems, build from source for now:

```sh
phpize
./configure --enable-terminal
make
make test
```

Then try the prompt example:

```sh
php -d extension=modules/terminal.so examples/prompt.php
```

For installed builds, use your normal `extension=terminal` configuration instead of `-d extension=...`.

### XAMPP on macOS

There is no prebuilt XAMPP macOS binary yet. Build it with XAMPP's PHP tools:

```sh
git clone https://github.com/prateekbhujel/php-terminal.git
cd php-terminal

/Applications/XAMPP/xamppfiles/bin/phpize
XAMPP_ARCH=$(/Applications/XAMPP/xamppfiles/bin/php -r 'echo php_uname("m");')
CFLAGS="-arch ${XAMPP_ARCH}" \
LDFLAGS="-arch ${XAMPP_ARCH}" \
./configure --with-php-config=/Applications/XAMPP/xamppfiles/bin/php-config --enable-terminal
make
make test
sudo make install
```

The `XAMPP_ARCH` step matters on Apple Silicon when XAMPP's PHP runs as `x86_64` under Rosetta.
Without matching that architecture, macOS may build an `arm64` `terminal.so` that XAMPP PHP cannot load.

Then edit:

```text
/Applications/XAMPP/xamppfiles/etc/php.ini
```

Add:

```ini
extension=terminal.so
```

Test it:

```sh
/Applications/XAMPP/xamppfiles/bin/php -m | grep terminal
/Applications/XAMPP/xamppfiles/bin/php examples/prompt.php
```

## Laravel Prompts and similar tools

The goal is to let Laravel Prompts behave on Windows the same way it behaves on macOS and Linux.

This extension is the native terminal layer for that. It does not monkey-patch Laravel Prompts, Symfony Console, or any other CLI framework by itself. Frameworks still need to opt in, but the hard part is exposed through one API: raw mode, safe terminal restore, single-key reads, terminal size, direct writes, and shared Unix/Windows key names.

A Laravel Prompts adapter would keep Laravel's existing prompt code, but swap the terminal backend when this extension is available:

- use `terminal_is_tty(TERMINAL_STDIN)` for interactivity checks
- use `terminal_enable_raw_mode()` and `terminal_restore_mode()` instead of `stty`
- use `terminal_read_key()` for input
- map extension key names back to Laravel Prompts' existing `Key::*` values
- use `terminal_get_size()` for columns and rows

The key mapping is intentionally small and predictable:

| `terminal_read_key()` | Laravel Prompts key |
| --- | --- |
| `up` | `Key::UP` |
| `down` | `Key::DOWN` |
| `left` | `Key::LEFT` |
| `right` | `Key::RIGHT` |
| `enter` | `Key::ENTER` |
| `backspace` | `Key::BACKSPACE` |
| `delete` | `Key::DELETE` |
| `escape` | `Key::ESCAPE` |
| `tab` | `Key::TAB` |
| `home` | first `Key::HOME` value |
| `end` | first `Key::END` value |
| `pageup` | `Key::PAGE_UP` |
| `pagedown` | `Key::PAGE_DOWN` |

Printable input is returned as the typed character, so normal text prompts do not need special handling.

Until Laravel Prompts has that adapter, existing Laravel Prompts releases will still use their current Windows fallback behavior. XAMPP and Windows users can test the same low-level prompt behavior today with `examples/prompt.php`.

The bundled `examples/prompt.php` file is intentionally small so framework authors can see the shape without reading a full TUI library.

Laravel Prompts integration is tracked separately in:

https://github.com/prateekbhujel/php-terminal/issues/18

Feedback for this alpha release is tracked in:

https://github.com/prateekbhujel/php-terminal/issues/16

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
