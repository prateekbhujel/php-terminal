# Core Direction

`terminal` is a package-first proving ground for small native terminal helpers in PHP CLI.

The goal is not to push a full terminal UI library into PHP core. The goal is to learn which low-level primitives are stable, cross-platform, and useful enough that PHP CLI apps should not need to keep rebuilding them in userland.

## Current shape

The public API intentionally looks closer to PHP core style after `v0.3.0`:

- one namespaced class: `Terminal\Terminal`
- finite values as enums: `Terminal\Backend`, `Terminal\Stream`, `Terminal\Key`
- narrow static methods instead of framework-specific abstractions
- opaque raw-mode tokens instead of exposing platform structs
- special keys as enum cases and printable input as strings

The old procedural API was removed while the package is still pre-1.0. That avoids carrying compatibility for a shape that does not match the current internals feedback.

## Principles

- Keep it primitive. No widgets, renderer, event loop, cursor abstraction, or prompt framework.
- Keep Unix and Windows under one PHP API.
- Prefer explicit enums where PHP can know the complete set of values.
- Return `false` for unavailable terminal capability, not guessed behavior.
- Restore terminal state before returning from key and secret reads.
- Keep examples small enough that maintainers can inspect the behavior quickly.
- Treat behavior as more important than package popularity.

## Open design questions

These are the places where feedback from PHP internals, framework maintainers, and Windows users matters most:

- Should `readKey()` return `Terminal\Key|string|false`, or should printable input be represented differently?
- Should printable Unicode input be one encoded code point, a grapheme cluster, or a lower-level byte sequence?
- Should `getSize()` only report real terminal size, or should it fall back to `COLUMNS` and `LINES`?
- Should raw mode be exposed as `enableRawMode()` / `restoreMode()`, or should key reads remain the only public raw-mode consumer?
- Should `readSecret()` return `false` for Escape/Ctrl-C/Ctrl-D, or distinguish abort from timeout?
- Should Windows key input expose more keys than the common prompt-navigation set?
- Should stream handling stay limited to stdin/stdout/stderr?

## Before 1.0

The package should not pretend to be done before these are tighter:

- Real Windows manual testing in PowerShell, Command Prompt, Windows Terminal, XAMPP, Laragon, and plain PHP zip builds.
- Adapter prototypes for at least one prompt library, even if upstream does not merge it yet.
- More issue reports from non-macOS terminals.
- Clear decision on `getSize()` environment fallback.
- Clear decision on abort vs timeout return behavior for `readSecret()`.
- PECL packaging path confirmed.
- Release notes that separate "package API" from "possible PHP core direction".

## What not to do

- Do not grow this into a TUI framework.
- Do not add Laravel/Symfony-specific public APIs.
- Do not add broad behavior just because it makes the package look bigger.
- Do not promise PHP core adoption. The package should provide evidence first.
