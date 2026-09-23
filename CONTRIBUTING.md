# Contributing

Contributions are welcome. The project stays deliberately narrow: native terminal
primitives for PHP CLI, not a full TUI framework. Small fixes with strong tests
are preferred over broad abstractions.

## Before opening an issue

For bugs, first check [compatibility](docs/compatibility.md) and run
`examples/doctor.php` if the extension loads. Include:

- operating system and terminal emulator
- PHP version, architecture and TS/NTS mode
- extension version and installation method
- the output from `php --ri terminal` and `examples/doctor.php`
- a minimal reproduction script
- what you expected and what happened instead

Use the bug report template when possible. Security-sensitive reports belong in
[SECURITY.md](SECURITY.md), not a public issue.

For API proposals, explain why the behavior needs a native extension rather than
portable userland PHP and how existing 1.x callers keep working.

## Development setup

Use the `phpize` and `php-config` that belong to the PHP binary you are
testing. Build in a path without spaces.

```sh
composer validate --strict
phpize
./configure --with-php-config="$(command -v php-config)"
make -j2
make test REPORT_EXIT_STATUS=1 NO_INTERACTION=1
```

On Apple Silicon, an x86_64 PHP distribution needs matching architecture flags:

```sh
CFLAGS="-arch x86_64" LDFLAGS="-arch x86_64" ./configure
```

Windows support is exercised by the native GitHub Actions matrix using
`php/php-windows-builder`. If a change is Windows-specific, describe how it was
tested on a real console in addition to CI.

## Tests

Behavior changes need PHPT coverage. Prefer tests that prove observable terminal
behavior, including terminal mode restoration and output, instead of only checking
that a function returned successfully.

Useful areas include:

- raw mode entry, nested/repeated use and restoration
- POSIX PTY behavior and delayed escape/UTF-8 sequences
- native Windows console events
- PHP stream resources and user-defined stream callbacks
- hidden input cancellation and failure paths
- high file-descriptor and memory-lifetime cases

CI runs PHP 8.1 through 8.5 on Linux, macOS coverage, Windows x64 TS/NTS builds,
and focused Valgrind checks. PHP nightly is advisory.

## Public API and generated files

New integrations should use `Io\Terminal\Terminal`. Its documented 1.x
signatures and behavior are the compatibility contract. Compatible fixes and
additions may ship in 1.x. Incompatible changes require a new major release.

Edit `terminal.stub.php` when changing PHP-visible signatures. The generated
`terminal_arginfo.h` must stay in sync and should not be hand-maintained as a
separate API definition.

The legacy `Terminal\Terminal` facade is retained for compatibility. Avoid
adding duplicate APIs there unless compatibility requires it.

## Pull requests

Keep pull requests focused. Before requesting review:

- add or update tests for behavior changes
- update README or docs when the public contract changes
- add an `Unreleased` changelog entry for user-visible changes
- run the local checks that apply to your platform
- avoid unrelated formatting or generated-file churn

Review comments may ask for a smaller scope or a platform-specific test. That is
normal for terminal code, where seemingly small changes can behave differently
across POSIX terminals and the Windows console.

See [docs/releasing.md](docs/releasing.md) for maintainer release steps.
