# Contributing

Contributions are welcome while `terminal` is still pre-1.0. The most useful
help is focused testing and small changes that improve cross-platform terminal
behavior without turning the extension into a full TUI toolkit.

Good contribution areas:

- Windows terminal behavior in PowerShell, Command Prompt, Windows Terminal,
  XAMPP, Laragon, WAMP, Herd, and plain PHP zip builds.
- Unix terminal behavior on Linux and macOS, including raw mode, resize events,
  ANSI capability detection, and UTF-8 key input.
- PHP stream-resource behavior for `STDIN`, `STDOUT`, `STDERR`, `php://`
  streams, and compatible custom streams.
- Adapter experiments for Laravel Prompts, Symfony Console, or similar CLI
  prompt libraries.
- Documentation that clarifies real installation or runtime behavior.

Read [docs/compatibility.md](./docs/compatibility.md) before changing platform
claims or adding new support notes.

When reporting an issue, include:

- operating system and terminal emulator
- PHP version, architecture, thread-safety mode, and compiler where relevant
- extension version
- whether PHP is from XAMPP, MAMP, Laragon, Herd, Homebrew, source, or php.net
- the output from `examples/doctor.php` if the extension loads
- a small reproduction script when possible

Before opening a pull request, run the checks that match your platform:

```sh
composer validate --strict
phpize
./configure
make
make test
```

Build from a path without spaces. PHP extension tooling rejects whitespace in
the build path before `configure` runs.

For macOS Apple Silicon with an x86_64 PHP distribution such as XAMPP, compile
with matching architecture flags:

```sh
CFLAGS="-arch x86_64" LDFLAGS="-arch x86_64" ./configure
```

Keep pull requests small and focused. If an API shape is uncertain, open an
issue first so the behavior can be discussed before code lands.
