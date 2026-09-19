<p align="center">
  <img src="https://raw.githubusercontent.com/prateekbhujel/php-terminal/main/docs/assets/php-terminal-mark.svg" alt="php-terminal" width="96" height="96">
</p>

# terminal for PHP CLI

<p align="center">
  <strong>Native terminal primitives for PHP CLI on Unix-like systems and Windows.</strong>
</p>

<p align="center">
  <a href="https://github.com/prateekbhujel/php-terminal/actions/workflows/ci.yml"><img src="https://img.shields.io/github/actions/workflow/status/prateekbhujel/php-terminal/ci.yml?branch=main&label=CI&logo=github" alt="CI status"></a>
  <a href="https://github.com/prateekbhujel/php-terminal/releases"><img src="https://img.shields.io/github/v/release/prateekbhujel/php-terminal?display_name=tag&sort=semver&logo=github" alt="Latest release"></a>
  <a href="https://packagist.org/packages/prateekbhujel/php-terminal"><img src="https://img.shields.io/packagist/v/prateekbhujel/php-terminal?label=Packagist&logo=packagist" alt="Packagist version"></a>
  <img src="https://img.shields.io/badge/PHP-%3E%3D8.1-777bb4?logo=php&logoColor=white" alt="PHP 8.1 or newer">
  <img src="https://img.shields.io/badge/type-php--ext-f28c18" alt="PHP extension">
  <a href="LICENSE"><img src="https://img.shields.io/github/license/prateekbhujel/php-terminal" alt="License"></a>
</p>

`terminal` is a PHP extension for terminal capabilities on Unix-like systems and Windows.

It exposes the pieces that are awkward to normalize in userland, especially once Windows enters the picture, without trying to become a full TUI toolkit: TTY detection, ANSI/VT support, terminal sizing, raw mode, key reads, secret input, direct writes, and safe restore handling.

Created and maintained by Pratik Bhujel.

Current release: `v0.9.0`.

`v0.9.0` aligns with core PHP API standards following review by Tim Düsterhus: unifying input/output streams into a cohesive `Terminal` session (`open()`, `create()`, `fromStreams()`), removing duplicate procedural functions to enforce RAII lifecycle safety, and adding the `Io\Terminal\TerminalSize` value object for dimensions. Full backward compatibility is preserved via `Terminal\*` legacy facades and aliases.

## Install

```sh
pie install prateekbhujel/php-terminal
```

Windows DLL builds are attached to each release for PHP 8.2-8.5 in x64 TS/NTS variants.

## Project status

`terminal` is pre-1.0 ecosystem work for PHP CLI tooling. API feedback, terminal compatibility reports, Windows testing, and focused pull requests are welcome. See [CONTRIBUTING.md](./CONTRIBUTING.md) for the most useful contribution areas.

## Compatibility

| Area | Current support |
| --- | --- |
| PHP | `>=8.1` |
| Unix-like systems | Linux CI plus POSIX backend support for macOS |
| Windows | Native x64 TS/NTS builds for PHP 8.2-8.5 |
| Install path | PIE, Windows release zips, or source builds with `phpize` |

See [docs/compatibility.md](./docs/compatibility.md) for platform boundaries, build notes, and stream-resource behavior.

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

## Current API

### 1. Object-Oriented Instance API (`Io\Terminal\Terminal`)

Encapsulates a unified interactive terminal session and guarantees terminal state restoration when instances go out of scope:

```php
use Io\Terminal\Terminal;
use Io\Terminal\Key;

// Open default controlling terminal session
$term = Terminal::open(); // or Terminal::create()
$token = $term->enableRawMode();

// Read keys with instant non-canonical single-character responsiveness
$key = $term->readKey();
if ($key === Key::Up) {
    // handled
}

// Automatically restored when $term or $token is destroyed, or explicitly:
$term->restoreMode();
```

Constructors:
- `Terminal::open(): Terminal` — Primary factory constructor for standard terminal session
- `Terminal::create(): Terminal` — Synonym for `Terminal::open()`
- `Terminal::fromStreams(mixed $input, mixed $output = null): Terminal` — Custom stream resources
- `Terminal::fromStream(mixed $stream): Terminal` — Single duplex stream resource

Instance methods:
- `$term->getInputStream(): mixed`
- `$term->getOutputStream(): mixed`
- `$term->getStream(): mixed`
- `$term->isTty(): bool`
- `$term->supportsAnsi(): bool`
- `$term->enableAnsi(): bool`
- `$term->getColorDepth(): ColorDepth`
- `$term->supportsColor(ColorDepth $depth = ColorDepth::Standard): bool`
- `$term->supportsTrueColor(): bool`
- `$term->getSize(): TerminalSize|false`
- `$term->setTitle(string $title): bool`
- `$term->beep(): bool`
- `$term->write(string $data): int|false`
- `$term->enableRawMode(): ModeToken|false`
- `$term->restoreMode(?ModeToken $mode = null): bool`
- `$term->readKey(?float $timeout = null, ?float $sequenceTimeout = null): Key|string|false`
- `$term->readSecret(string $prompt = ''): string`

### 2. Dimension Value Object (`Io\Terminal\TerminalSize`)

Returned by `$term->getSize()`:
- `$size->cols`: int
- `$size->rows`: int
- `$size->width`: int (alias for `cols`)
- `$size->height`: int (alias for `rows`)
- `$size->toArray()`: `array{cols: int, rows: int}`

### 3. Enums (`Io\Terminal\*`)

- `Backend`: `Posix`, `Windows`
- `Stream`: `Stdin`, `Stdout`, `Stderr`
- `ColorDepth`: `None` (0-bit), `Standard` (4-bit), `Extended` (8-bit), `TrueColor` (24-bit). Helper method: `$depth->bits(): int`.
- `Key`: `Up`, `Down`, `Left`, `Right`, `Enter`, `Backspace`, `Escape`, `Tab`, `Home`, `End`, `Delete`, `PageUp`, `PageDown`, `Resize`, `F1` through `F12`

### 4. Legacy Facade (`Terminal\Terminal`)

Full backward compatibility is preserved for existing code using the `Terminal\*` namespace:

```php
use Terminal\Terminal;
use Terminal\Key;

$cols = Terminal::getWidth();
$key = Terminal::readKey();
```

Current key input scope:

- normalized keys: arrows, enter, backspace, escape, tab (including Shift+Tab / BackTab `\e[Z`), home, end, delete, page up, page down, resize
- function keys: F1-F12 on Windows and Unix terminals that emit SS3, CSI, or CSI tilde sequences
- printable input: returned as a string containing the next encoded code point
- control bytes such as Ctrl+C: returned as single-byte strings by `readKey()`
- not normalized yet: modifier combinations and full grapheme clusters
- unknown POSIX escape sequences: fall back to `Terminal\Key::Escape`

The earlier procedural API was removed while the project is still pre-1.0 so the extension can track the PHP core discussion more closely.

## Migrating from 0.1.x

The 0.1.x procedural API and `TERMINAL_*` constants were removed in favor of the namespaced class and enum API.

| 0.1.x | 0.6.0 |
| --- | --- |
| `terminal_backend()` | `Terminal\Terminal::getBackend()` |
| `terminal_is_tty(TERMINAL_STDOUT)` | `Terminal\Terminal::isTty(Terminal\Stream::Stdout)` |
| `terminal_supports_ansi(TERMINAL_STDOUT)` | `Terminal\Terminal::supportsAnsi(Terminal\Stream::Stdout)` |
| `terminal_enable_ansi(TERMINAL_STDOUT)` | `Terminal\Terminal::enableAnsi(Terminal\Stream::Stdout)` |
| `terminal_get_size(TERMINAL_STDOUT)` | `Terminal\Terminal::getSize(Terminal\Stream::Stdout)` |
| `terminal_write($data, TERMINAL_STDOUT)` | `Terminal\Terminal::write($data, Terminal\Stream::Stdout)` |
| `terminal_enable_raw_mode(TERMINAL_STDIN)` | `Terminal\Terminal::enableRawMode(Terminal\Stream::Stdin)` |
| `terminal_restore_mode($mode)` | `Terminal\Terminal::restoreMode($mode)` |
| `terminal_read_key($timeout)` | `Terminal\Terminal::readKey($timeout)` |
| `terminal_read_secret()` | `Terminal\Terminal::readSecret()` |
| `TERMINAL_STDIN` | `Terminal\Stream::Stdin` |
| `TERMINAL_STDOUT` | `Terminal\Stream::Stdout` |
| `TERMINAL_STDERR` | `Terminal\Stream::Stderr` |

Example:

```php
use Terminal\Key;
use Terminal\ModeToken;
use Terminal\Stream;
use Terminal\Terminal;

Terminal::enableAnsi(Stream::Stdout);

$mode = Terminal::enableRawMode();
try {
    $key = Terminal::readKey();

    if ($key === Key::Up) {
        Terminal::write("up\n");
    } elseif (is_string($key)) {
        Terminal::write("typed {$key}\n");
    }
} finally {
    if ($mode instanceof ModeToken) {
        Terminal::restoreMode($mode);
    }
}
```

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

## Installing v0.6.0

Install with PIE:

```sh
pie install prateekbhujel/php-terminal
```

The `v0.6.0` release is available at:

https://github.com/prateekbhujel/php-terminal/releases/tag/v0.6.0

Windows builds are attached for PHP 8.2-8.5, x64, TS/NTS. These are native Windows builds for normal Windows PHP runtimes, not WSL. Pick the zip that matches your PHP version and thread-safety mode, copy `php_terminal.dll` into your PHP extension directory, and enable it with:

```ini
extension=php_terminal.dll
```

Build from source on Unix-like systems:

```sh
git clone https://github.com/prateekbhujel/php-terminal.git
cd php-terminal
git checkout v0.6.0
phpize
./configure
make
make test
```

Then try the stream-resource and prompt examples:

```sh
php -d extension=modules/terminal.so examples/streams.php
php -d extension=modules/terminal.so examples/prompt.php
```

For installed builds, use your normal `extension=terminal` configuration instead of `-d extension=...`.

### Build current main from source

To test unreleased changes after `v0.6.0`:

```sh
phpize
./configure
make
make test
```

Then try the stream-resource and prompt examples:

```sh
php -d extension=modules/terminal.so examples/streams.php
php -d extension=modules/terminal.so examples/prompt.php
```

For installed builds, use your normal `extension=terminal` configuration instead of `-d extension=...`.

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

- PHP 8.2, thread safety disabled: `php_terminal-v0.6.0-8.2-nts-vs16-x86_64.zip`
- PHP 8.2, thread safety enabled: `php_terminal-v0.6.0-8.2-ts-vs16-x86_64.zip`
- PHP 8.4, thread safety disabled: `php_terminal-v0.6.0-8.4-nts-vs17-x86_64.zip`

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
    ./configure --with-php-config="${PHP_CONFIG}"
else
    ./configure --with-php-config="${PHP_CONFIG}"
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

- use `Terminal\Terminal::isTty(Terminal\Stream::Stdin)` for interactivity checks
- use `Terminal\Terminal::enableAnsi()` before rendering ANSI prompts on Windows
- use `Terminal\Terminal::enableRawMode()` and `Terminal\Terminal::restoreMode()` instead of `stty`
- use `Terminal\Terminal::readKey()` for input
- use `Terminal\Terminal::readSecret()` for password/secret prompts instead of shelling out to platform-specific helpers
- map `Terminal\Key::*` cases back to Laravel Prompts' existing `Key::*` values
- use `Terminal\Terminal::getSize()` for columns and rows

The key mapping is intentionally small and predictable:

| `Terminal\Terminal::readKey()` | Laravel Prompts key |
| --- | --- |
| `Terminal\Key::Up` | `Key::UP` |
| `Terminal\Key::Down` | `Key::DOWN` |
| `Terminal\Key::Left` | `Key::LEFT` |
| `Terminal\Key::Right` | `Key::RIGHT` |
| `Terminal\Key::Enter` | `Key::ENTER` |
| `Terminal\Key::Backspace` | `Key::BACKSPACE` |
| `Terminal\Key::Delete` | `Key::DELETE` |
| `Terminal\Key::Escape` | `Key::ESCAPE` |
| `Terminal\Key::Tab` | `Key::TAB` |
| `Terminal\Key::Home` | first `Key::HOME` value |
| `Terminal\Key::End` | first `Key::END` value |
| `Terminal\Key::PageUp` | `Key::PAGE_UP` |
| `Terminal\Key::PageDown` | `Key::PAGE_DOWN` |

Printable input is returned as the typed character, so normal text prompts do not need special handling.

Until Laravel Prompts has that adapter, existing Laravel Prompts releases will still use their current Windows fallback behavior. XAMPP and Windows users can test the same low-level prompt behavior today with `examples/prompt.php`.

The bundled `examples/prompt.php` file is intentionally small so framework authors can see the shape without reading a full TUI library.

Future Laravel Prompts adapter work should target the `Terminal\Terminal` API from `v0.6.0`.

For release feedback, open a new issue with the OS, terminal, PHP version, extension version, what you tried, and the behavior you expected.

## Build

### Unix-like systems

```sh
phpize
./configure
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

use Terminal\Terminal;
use Terminal\ModeToken;

var_dump(Terminal::getBackend());
var_dump(Terminal::isTty());
var_dump(Terminal::supportsAnsi());
var_dump(Terminal::getSize());
Terminal::write("hello from terminal\n");

$mode = Terminal::enableRawMode();
if ($mode instanceof ModeToken) {
    try {
        $key = Terminal::readKey(0.5);
    } finally {
        Terminal::restoreMode($mode);
    }
}
```

There are runnable examples in [`examples/basic.php`](./examples/basic.php), [`examples/doctor.php`](./examples/doctor.php), [`examples/prompt.php`](./examples/prompt.php), and [`examples/streams.php`](./examples/streams.php).
