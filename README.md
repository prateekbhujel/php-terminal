# terminal

`terminal` is a PHP extension for basic terminal capabilities on Unix-like systems and Windows.

The first cut stays small on purpose. It exposes the pieces that are awkward to normalize in userland, especially once Windows enters the picture, without trying to become a full TUI toolkit.

## Current API

- `terminal_backend(): string`
- `terminal_is_tty(int $stream = TERMINAL_STDOUT): bool`
- `terminal_supports_ansi(int $stream = TERMINAL_STDOUT): bool`
- `terminal_enable_ansi(int $stream = TERMINAL_STDOUT): bool`
- `terminal_get_size(int $stream = TERMINAL_STDOUT): array{columns:int, rows:int}|false`
- `terminal_write(string $data, int $stream = TERMINAL_STDOUT): int|false`
- `terminal_enable_raw_mode(int $stream = TERMINAL_STDIN): string|false`
- `terminal_restore_mode(string $mode): bool`
- `terminal_read_key(?float $timeout = null): string|false`
- `terminal_read_secret(?float $timeout = null): string|false`

`terminal_enable_ansi()` enables ANSI/VT output on Windows stdout/stderr and is a no-op capability check on Unix-like terminals.
`terminal_write()` accepts `TERMINAL_STDOUT` and `TERMINAL_STDERR`.
`terminal_enable_raw_mode()` currently accepts `TERMINAL_STDIN` and returns an opaque mode token that should be passed back to `terminal_restore_mode()`.
`terminal_enable_raw_mode()` leaves terminal output processing intact, so normal prompt output such as `"\n"` keeps working while input is read one key at a time.
`terminal_read_key()` temporarily prepares standard input for key reads, returns printable keys as-is including UTF-8 input, named keys as strings like `up`, `down`, `left`, `right`, `enter`, `backspace`, `escape`, and `tab`, restores the previous mode before returning, and returns `false` when no key is available before the timeout.
`terminal_read_secret()` reads a hidden line from standard input, restores the previous mode before returning, handles backspace and UTF-8 input, and returns `false` on timeout or abort.

Constants:

- `TERMINAL_STDIN`
- `TERMINAL_STDOUT`
- `TERMINAL_STDERR`

## Why this exists

PHP already has useful pieces such as `stream_isatty()` and `sapi_windows_vt100_support()`, but there is still no small extension that exposes a shared terminal capability layer across Unix and Windows.

The main goal is native Windows parity for PHP CLI prompts and terminal apps. Users should not need WSL just to get arrow keys, raw mode, terminal size, and safe restore behavior that already work on macOS and Linux.

This also removes two common framework workarounds: spawning `stty`/`mode CON` helpers for terminal state, and bundling a Windows-only helper executable just to read hidden password input.

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

## Installing 0.2.0

The `v0.2.0` release is available at:

https://github.com/prateekbhujel/php-terminal/releases/tag/v0.2.0

Windows builds are attached for PHP 8.2-8.5, x64, TS/NTS. These are native Windows builds for normal Windows PHP runtimes, not WSL. Pick the zip that matches your PHP version and thread-safety mode, copy `php_terminal.dll` into your PHP extension directory, and enable it with:

```ini
extension=php_terminal.dll
```

### Windows PHP distributions

XAMPP, MAMP, WAMP, Laragon, Herd, and plain downloaded PHP builds work as long as the DLL matches the PHP build your app actually runs.

Example PHP binaries:

- XAMPP: `C:\xampp\php\php.exe`
- MAMP: `C:\MAMP\bin\php\php8.x.x\php.exe`
- Laragon: `C:\laragon\bin\php\php-8.x.x\php.exe`
- Plain PHP zip: `C:\php\php.exe`

Check the PHP version, thread-safety mode, architecture, and compiler:

```bat
set PHP_BIN=C:\xampp\php\php.exe
%PHP_BIN% -i | findstr /C:"PHP Version" /C:"Thread Safety" /C:"Architecture" /C:"Compiler"
%PHP_BIN% --ini
```

Download the matching zip from the release page. For example:

- PHP 8.2, thread safety disabled: `php_terminal-v0.2.0-8.2-nts-vs16-x86_64.zip`
- PHP 8.2, thread safety enabled: `php_terminal-v0.2.0-8.2-ts-vs16-x86_64.zip`
- PHP 8.4, thread safety disabled: `php_terminal-v0.2.0-8.4-nts-vs17-x86_64.zip`

Copy `php_terminal.dll` into that PHP installation's extension directory, for example:

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
%PHP_BIN% -m | findstr terminal
%PHP_BIN% examples\doctor.php
%PHP_BIN% examples\prompt.php
```

Once enabled in that PHP runtime, any CLI app using the same `php.exe` can detect and use `terminal`. The app or framework still needs integration code; the extension provides the native Windows terminal primitives.

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

### Install into a specific PHP distribution

Each PHP installation has its own extension directory and `php.ini`. Build `terminal` with the `phpize` and `php-config` that belong to the PHP binary your application actually runs.

Example target PHP binaries:

- XAMPP on macOS: `/Applications/XAMPP/xamppfiles/bin/php`
- MAMP on macOS: `/Applications/MAMP/bin/php/php8.x.x/bin/php`
- LAMPP/XAMPP on Linux: `/opt/lampp/bin/php`
- Homebrew or system PHP: `$(command -v php)`

Build for that target PHP:

```sh
git clone https://github.com/prateekbhujel/php-terminal.git
cd php-terminal

PHP_BIN=/Applications/XAMPP/xamppfiles/bin/php
PHPIZE="$(dirname "${PHP_BIN}")/phpize"
PHP_CONFIG="$(dirname "${PHP_BIN}")/php-config"

"${PHPIZE}"

if [ "$(uname -s)" = "Darwin" ]; then
    PHP_ARCH=$("${PHP_BIN}" -r 'echo php_uname("m");')
    CFLAGS="-arch ${PHP_ARCH}" \
    LDFLAGS="-arch ${PHP_ARCH}" \
    ./configure --with-php-config="${PHP_CONFIG}" --enable-terminal
else
    ./configure --with-php-config="${PHP_CONFIG}" --enable-terminal
fi

make
make test
sudo make install
```

Build from a path without spaces. The PHP extension build tooling rejects
whitespace in the build path before `configure` runs.

The macOS architecture step matters on Apple Silicon when a PHP distribution runs as `x86_64` under Rosetta. Without matching that architecture, macOS may build an `arm64` `terminal.so` that the target PHP cannot load.

Find the right `php.ini`:

```sh
"${PHP_BIN}" --ini
```

Add this to that PHP installation's loaded `php.ini`:

```ini
extension=terminal.so
```

Test it:

```sh
"${PHP_BIN}" -m | grep terminal
"${PHP_BIN}" examples/doctor.php
"${PHP_BIN}" examples/prompt.php
```

Once enabled there, any PHP application using that same PHP binary can use `terminal`. If another app uses a different PHP binary, build/install the extension for that PHP too.

## Laravel Prompts and similar tools

The goal is to let Laravel Prompts and similar prompt libraries behave on native Windows the same way they behave on macOS and Linux.

This extension is the native terminal layer for that. It does not monkey-patch Laravel Prompts, Symfony Console, or any other CLI framework by itself. Frameworks still need to opt in, but the hard part is exposed through one API: raw mode, safe terminal restore, single-key reads, terminal size, direct writes, and shared Unix/Windows key names.

That means the end-user path should be: install the matching Windows `php_terminal.dll`, enable it in the PHP used by the app, install or use an adapter that detects `terminal`, then run prompts from PowerShell, Command Prompt, or Windows Terminal without WSL.

A Laravel Prompts adapter would keep Laravel's existing prompt code, but swap the terminal backend when this extension is available:

- use `terminal_is_tty(TERMINAL_STDIN)` for interactivity checks
- use `terminal_enable_ansi()` before rendering ANSI prompts on Windows
- use `terminal_enable_raw_mode()` and `terminal_restore_mode()` instead of `stty`
- use `terminal_read_key()` for input
- use `terminal_read_secret()` for password/secret prompts instead of shelling out to platform-specific helpers
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

Feedback for this release is tracked in:

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
