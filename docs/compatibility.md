# Compatibility

`terminal` is a native PHP extension. It has to match the PHP binary and
platform that load it.

## Supported PHP versions

| Runtime | Status |
| --- | --- |
| PHP 8.1 | Tested on Unix CI |
| PHP 8.2 | Tested on Unix CI and Windows CI |
| PHP 8.3 | Tested on Unix CI and Windows CI |
| PHP 8.4 | Tested on Unix CI and Windows CI |
| PHP 8.5 | Tested on Unix CI and Windows CI |
| PHP nightly | Built and tested on Unix CI, allowed to fail if upstream changes break temporarily |

The package metadata requires PHP `>=8.1`.

## Platforms

| Platform | Status |
| --- | --- |
| Linux | Tested through the Unix CI build |
| macOS | Supported through the POSIX backend |
| Windows | Tested through native Windows CI builds for PHP 8.2-8.5, x64, TS/NTS |
| WSL | Should use the POSIX backend, but the goal is native Windows support without requiring WSL |

## Installation paths

| Method | Notes |
| --- | --- |
| PIE | Preferred install path: `pie install prateekbhujel/php-terminal` |
| Windows release zip | Use the zip that matches PHP version, architecture, compiler, and thread-safety mode |
| Source build | Use the `phpize` and `php-config` from the PHP binary that will load the extension |

Build from a path without spaces. PHP extension tooling rejects whitespace in
the build path before `configure` runs.

On Apple Silicon, some PHP distributions still run as `x86_64` under Rosetta.
Compile with matching architecture flags when the target PHP is `x86_64`.

## API scope

Stable enough to test against:

- TTY detection
- ANSI/VT support detection and enablement
- terminal size detection
- raw mode enable/restore through `Terminal\ModeToken`
- key reads for common prompt keys
- secret input
- direct writes
- PHP stream-resource support for native streams

Still intentionally narrow:

- not a full TUI toolkit
- no ncurses dependency
- no framework integration code bundled in this repository
- no promise yet to preserve every pre-1.0 API detail

## Stream resources

Stream-aware methods accept either `Terminal\Stream` enum cases or PHP stream
resources. A resource must be backed by a native descriptor or Windows handle.
Unsupported wrappers return `false` or raise the normal PHP type/resource error.

Good candidates:

- `STDIN`
- `STDOUT`
- `STDERR`
- `fopen('php://stdin', 'r')`
- `fopen('php://stdout', 'w')`
- `fopen('php://stderr', 'w')`

Use `examples/streams.php` for a small non-interactive check.
