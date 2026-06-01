# Changelog

## 0.2.0 - 2026-06-01

- Added `terminal_enable_ansi()` for ANSI/VT output enablement.
- Added `terminal_read_secret()` for hidden password/secret input.
- Hardened raw-mode behavior so prompt output keeps normal newline processing.
- Hardened Windows key reads so console mode is restored after each read.
- Added complete UTF-8 key reads on POSIX pseudo terminals.
- Hardened stream writes against zero-byte progress and partial Windows writes.
- Expanded pseudo-terminal regression coverage for common keys, UTF-8 keys, raw-mode output processing, and secret input.
- Added release documentation for Laravel Prompts/Symfony-style integration needs.

## 0.1.0 - 2026-05-24

- First public release.
- Added backend detection, TTY detection, ANSI detection, terminal sizing, direct writes, raw-mode save/restore, and key input.
- Added Unix and Windows build support.
