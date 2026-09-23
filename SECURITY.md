# Security Policy

`terminal` exposes local CLI terminal primitives. Security-sensitive behavior
includes memory safety, secret-input disclosure, terminal control-sequence
injection, use of the wrong stream or console handle, and failures that leave a
terminal mode changed after an operation aborts.

## Supported versions

| Version | Supported |
| --- | --- |
| Latest 1.x | Yes |
| Older 1.x | Upgrade to the latest patch release |
| < 1.0 | No |

Security fixes are released on the current 1.x line. Pre-1.0 releases do not
receive fixes.

## Reporting

Please report a suspected vulnerability privately by emailing
prateekbhujelpb@gmail.com. Do not open a public issue for a vulnerability before
a fix or disclosure plan is agreed.

Include, when available:

- operating system and terminal emulator
- PHP version, architecture and TS/NTS mode
- extension version and installation method
- a minimal reproduction
- expected security impact
- whether the issue requires a TTY, a specific stream type, or Windows console input

Reports about ordinary installation failures, API design, non-security crashes,
or compatibility questions should use GitHub issues instead.

## Scope notes

The extension should not expose hidden input, emit control sequences from
untrusted title text, access terminal handles outside the configured session, or
leave raw/console modes changed after recoverable failures. Uncatchable process
termination and runtime crashes cannot run normal cleanup and are documented
limitations rather than a security guarantee.
