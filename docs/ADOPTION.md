# Adoption Notes

`terminal` is useful when a CLI app already has prompts or console rendering, but needs native terminal primitives that work the same way on Unix-like systems and Windows.

## Detection

```php
if (!extension_loaded('terminal')) {
    // Keep the existing framework fallback.
}
```

For optional integration, detect the extension and use it only for the low-level terminal backend. Do not require users to install it unless the app has a hard native-terminal requirement.

## Adapter checklist

An adapter for a prompt or console package usually needs to map these pieces:

- interactivity: `Terminal\Terminal::isTty(Terminal\Stream::Stdin)`
- ANSI output: `Terminal\Terminal::enableAnsi(Terminal\Stream::Stdout)`
- terminal size: `Terminal\Terminal::getSize(Terminal\Stream::Stdout)`
- raw state: `Terminal\Terminal::enableRawMode()` and `Terminal\Terminal::restoreMode()`
- key input: `Terminal\Terminal::readKey()`
- secret input: `Terminal\Terminal::readSecret()`
- output: `Terminal\Terminal::write($data, Terminal\Stream::Stdout)`

## Key mapping

`Terminal\Terminal::readKey()` returns `Terminal\Key` enum cases for known control keys and strings for printable input.

Common prompt mapping:

| Terminal key | Prompt meaning |
| --- | --- |
| `Terminal\Key::Up` | move selection up |
| `Terminal\Key::Down` | move selection down |
| `Terminal\Key::Left` | move cursor left |
| `Terminal\Key::Right` | move cursor right |
| `Terminal\Key::Enter` | submit |
| `Terminal\Key::Backspace` | delete previous character |
| `Terminal\Key::Delete` | delete next character |
| `Terminal\Key::Escape` | cancel |
| `Terminal\Key::Tab` | complete or move focus |
| `Terminal\Key::Home` | start |
| `Terminal\Key::End` | end |
| `Terminal\Key::PageUp` | page up |
| `Terminal\Key::PageDown` | page down |
| string | append printable input |

## Manual test matrix

The most valuable reports are from real interactive terminals:

- Windows Terminal + PowerShell
- Windows Terminal + Command Prompt
- classic Command Prompt
- XAMPP PHP CLI
- Laragon PHP CLI
- plain PHP zip CLI
- macOS Terminal
- iTerm2
- common Linux terminals

Run:

```sh
php examples/doctor.php
php examples/prompt.php
php examples/key-inspector.php
```

Report the PHP version, terminal app, shell, OS, extension version, and exact key or prompt behavior.

## Integration rule

Keep the adapter boring. The extension should provide terminal facts and input primitives; the framework should keep ownership of rendering, validation, history, default values, and prompt semantics.
