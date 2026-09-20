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

Current release: `v1.0.0`.

## Install

```sh
pie install prateekbhujel/php-terminal:^1.0
php --ri terminal
```

For Windows, download the DLL zip matching your PHP version, architecture and
TS/NTS mode from [the release page](https://github.com/prateekbhujel/php-terminal/releases/tag/v1.0.0).
Copy `php_terminal.dll` to that PHP installation's extension directory and enable
`extension=php_terminal.dll` in its `php.ini`. Check the selected runtime with
`php --ini` and `php -i`; XAMPP, MAMP, Laragon and other distributions each have
their own PHP configuration.

PHP 8.1 or later is required. CI covers Linux PHP 8.1–8.5, macOS PHP 8.4, and
Windows x64 PHP 8.2–8.5 in TS/NTS builds. PHP nightly is checked separately and
is advisory. See [compatibility](docs/compatibility.md) for platform boundaries.

## A terminal session

```php
use Io\Terminal\Key;
use Io\Terminal\Terminal;

$terminal = Terminal::create(); // Standard input and standard output.
$size = $terminal->getSize();
if ($size !== false) {
    echo "{$size->cols} columns, {$size->rows} rows\n";
}

$key = $terminal->readKey(1.0);
if ($key === Key::Up) {
    echo "Up\n";
}
```

`Io\Terminal\Terminal` is the API for new integrations. Its public signatures,
enums and documented behavior are the stable 1.x contract. Compatible additions
and bug fixes may ship in 1.x; incompatible API changes require a new major
release. Experimental PHP nightly behavior is outside that compatibility promise.

The extension supplies terminal operations, not a TUI toolkit. Frameworks must
opt in; installing it does not automatically change Symfony Console or Laravel
Prompts. Native input avoids spawning `stty` or bundling a hidden-input executable.
An RFC for PHP core is planned; this extension remains independently usable.

## Input and output streams

```php
$terminal = Terminal::fromStreams(STDIN, STDERR);
$duplex = Terminal::fromStream($stream);
```

- `create()` and its alias `open()` use standard input and standard output. They
  do not open a separate controlling terminal.
- `fromStreams($input, $output = null)` uses `$input` for both directions when
  output is omitted or null. Supply an output explicitly for an input-only stream.
- `fromStream($stream)` uses one stream for both directions.
- `new Terminal()` uses standard input/output; with one non-null input, output
  defaults to that input. Stream values may be PHP resources or `Io\Terminal\Stream`
  enum cases. `getInputStream()` and `getOutputStream()` return those values;
  `getStream()` returns the output value on an initialized session.

`readKey()`, `readSecret()` and raw mode operate on input. `isTty()`, dimensions,
ANSI/color operations, title, bell and writes operate on output. Use
`stream_isatty($input)` to check a resource used for input.

Native input requires a TTY descriptor or Windows console input handle. Pipes
and memory streams are useful outputs but cannot supply native terminal events.
`write()` and explicit prompts support writable PHP streams, including wrappers.

## Hidden input

```php
$terminal = Terminal::fromStreams(STDIN, STDERR);
try {
    $secret = $terminal->readSecret('Password: ');
    $terminal->write("\n");
    // Use $secret without logging it.
} catch (\RuntimeException $e) {
    $terminal->write("\nUnable to read hidden input.\n");
}
```

`readSecret(string $prompt = ''): string` emits no characters, masks, backspace
sequences or newline. An explicit prompt goes to the session output; an empty
prompt writes nothing. Empty input returns `''`; whitespace is preserved.
Backspace removes one encoded code point, not an entire grapheme cluster.

Ctrl+C, Ctrl+D and Escape cancel with `\RuntimeException`. Unavailable input,
write/read failures and failed mode restoration also raise `\RuntimeException`.
Invalid arguments and closed resources retain PHP's `\TypeError` / `\ValueError`
behavior. Exceptions from user-defined stream callbacks propagate unchanged.

Both reads restore the previous input mode before returning, including an outer
raw mode. A failed restore is reported. On POSIX, pending PHP-buffered bytes are
consumed first. Windows uses console key events: mixing PHP byte reads with
native input is unsupported, and pending PHP-buffered bytes cause a read failure.

## Keys and raw mode

`readKey(?float $timeout = null, ?float $sequenceTimeout = null): Key|string|false`
returns a named key, a printable encoded code point, or `false` on timeout or
unavailable input. Null timeout blocks; zero polls. On POSIX the default timeout
between escape/UTF-8 bytes is 25 ms; `sequenceTimeout` adjusts it. Windows uses
native events and ignores that argument.

Named keys are arrows, Enter, Tab, Backspace, Escape, Home, End, Delete, PageUp,
PageDown, Resize and F1–F12. Shift+Tab maps to Tab. Other control bytes remain
strings. Unknown POSIX escape sequences map to Escape. Modifier combinations
and full grapheme clusters are not normalized.

For a prompt loop, keep one session alive:

```php
$mode = $terminal->enableRawMode();
if ($mode === false) {
    throw new \RuntimeException('Cannot enable raw mode.');
}
try {
    while (($key = $terminal->readKey()) !== false) {
        if ($key === Key::Enter || $key === Key::Escape || $key === "\x03") {
            break;
        }
    }
} finally {
    if (!$terminal->restoreMode()) {
        throw new \RuntimeException('Cannot restore terminal mode.');
    }
}
```

Repeated `enableRawMode()` calls on one session return its active `ModeToken`.
`restoreMode(?ModeToken $mode = null): bool` restores the original mode and consumes
the token. Tokens cannot be cloned, serialized or reused after restoration.
The session retains its token, so unsetting only the caller's token variable does
not restore the session. Destruction of the session also attempts restoration,
even if the caller still holds the token.

Keep resource-backed input open until restored. Use one owner per terminal;
independent sessions or other native code must coordinate changes and restore in
reverse order. Cleanup runs during normal destruction and PHP shutdown, including
uncaught exceptions, but cannot run after an uncatchable kill or runtime crash.
Destructor cleanup cannot report restoration failure; use explicit restoration
when the application must check it. Raw mode preserves output processing.

## Output capabilities

| Method | Result |
| --- | --- |
| `isTty()` | Whether the output is a terminal |
| `supportsAnsi()` / `enableAnsi()` | Detect / enable ANSI or Windows VT support |
| `getSize()` | `TerminalSize` or `false`; positive `COLUMNS` and `LINES` are a fallback |
| `getColorDepth()` | `ColorDepth::None`, `Standard`, `Extended` or `TrueColor` |
| `supportsColor(ColorDepth $depth = ColorDepth::Standard)` | Whether that depth is available |
| `supportsTrueColor()` | Whether 24-bit color is available |
| `write(string $data)` | Bytes written, or `false` on failure; an unsuccessful write may be partial |
| `setTitle(string $title)` / `beep()` | Success as a boolean |

`TerminalSize` has readonly `cols` / `rows` and `width` / `height` aliases;
`toArray()` returns `['cols' => ..., 'rows' => ...]`.
`Backend`, `Stream`, `ColorDepth` and `Key` are unbacked enums: use `->name`,
not `->value`. `ColorDepth::bits()` returns 0, 4, 8 or 24.
`Terminal::getBackend()` returns `Backend::Posix` or `Backend::Windows`.
See [the stub](terminal.stub.php) for every signature.

## Upgrading to 1.0

This is the first stable API release, with intentional changes from pre-1.0:

- Keys and secrets now read the configured input rather than process stdin.
- Hidden input is silent. Render masks and newlines in the application if needed.
- Secret cancellation and operational failure throw `RuntimeException`, replacing
  `Error`. Update catches accordingly.
- `fromStreams()` requires an input argument. A live session cannot be
  reinitialized, and another Terminal object is not a stream argument.
- Repeated raw-mode entry is idempotent. Explicit restore failures retain state
  so restoration can be retried while the resource remains valid.

The v0.9 session API, unbacked enums and dimensions object remain. Code from
0.8 or earlier must account for the session and enum changes too. The
`Terminal\Terminal` static facade and `Terminal\*` aliases remain available;
its `getSize()` returns an array and its hidden input uses the same 1.0 behavior.
No fixes are backported to pre-1.0 releases. See [the changelog](CHANGELOG.md).

## Build from source

Use the `phpize` and `php-config` belonging to the PHP binary that will load the
extension. Build in a path without spaces.

```sh
git clone https://github.com/prateekbhujel/php-terminal.git
cd php-terminal
git checkout v1.0.0
phpize
./configure --with-php-config="$(command -v php-config)"
make -j2
make test REPORT_EXIT_STATUS=1 NO_INTERACTION=1
sudo make install
```

Enable `extension=terminal.so` in that runtime's `php.ini`, then run
`php --ri terminal`. On Apple Silicon with an x86_64 PHP distribution, configure
with `CFLAGS="-arch x86_64" LDFLAGS="-arch x86_64"` to match PHP's architecture.
Windows source builds use `config.w32` and the PHP SDK; CI uses
`php/php-windows-builder`.

Run an example against a local build:

```sh
php -d extension=modules/terminal.so examples/doctor.php
php -d extension=modules/terminal.so examples/prompt.php
```

[Basic operations](examples/basic.php), [stream output](examples/streams.php),
[diagnostics](examples/doctor.php) and [a prompt loop](examples/prompt.php) use the
session API. Read [CONTRIBUTING.md](CONTRIBUTING.md) for tests and bug reports and
[SECURITY.md](SECURITY.md) for private security reports.
