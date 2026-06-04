---
name: Windows manual test report
about: Share results from real Windows terminal testing
title: 'Windows test: '
labels: windows, testing
assignees: ''
---

## Environment

- Windows version:
- Terminal app: Windows Terminal / PowerShell / Command Prompt / other
- PHP distribution: XAMPP / Laragon / plain PHP zip / other
- PHP version:
- Thread safety: TS / NTS
- `terminal` DLL zip used:

## Checks

- [ ] `php -m` shows `terminal`
- [ ] `php examples/doctor.php` works
- [ ] `php examples/prompt.php` handles arrows, Enter, Escape, and text
- [ ] `php examples/key-inspector.php` reports expected keys
- [ ] secret input does not echo typed text
- [ ] terminal mode restores after timeout, Escape, and Ctrl-C

## Notes

Paste any unexpected output or behavior here.
