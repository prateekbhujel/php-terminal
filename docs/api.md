# API guide

The stable API for new code is the `Io\Terminal` namespace. The documented
1.x signatures and behavior form the compatibility contract.

## Sessions

A `Terminal` object owns an input/output terminal session.

| API | Purpose |
| --- | --- |
| `Terminal::create()` / `open()` | Standard input and standard output |
| `Terminal::fromStreams($input, $output = null)` | Separate or shared PHP streams |
| `Terminal::fromStream($stream)` | One stream for both directions |
| `new Terminal(...)` | Constructor form of the same session setup |
| `getInputStream()` / `getOutputStream()` | Return the configured values |
| `getStream()` | Return the output side of an initialized session |

Input operations use the configured input. TTY/capability queries, writes, title,
bell and dimensions use the configured output.

## Output and capabilities

| API | Result |
| --- | --- |
| `Terminal::getBackend()` | `Backend::Posix` or `Backend::Windows` |
| `isTty()` | Whether the output targets a terminal |
| `supportsAnsi()` | Whether ANSI/VT output is available |
| `enableAnsi()` | Enable VT processing where supported |
| `getSize()` | `TerminalSize` or `false` |
| `getColorDepth()` | `ColorDepth` enum |
| `supportsColor($depth)` | Capability check for a requested depth |
| `supportsTrueColor()` | Convenience 24-bit color check |
| `write($data)` | Byte count or `false` |
| `setTitle($title)` | Safe terminal title update as a boolean |
| `beep()` | Terminal bell as a boolean |

`setTitle()` accepts valid UTF-8 and rejects C0/C1 controls, DEL and malformed
UTF-8 instead of emitting an unsafe control sequence.

`TerminalSize` exposes readonly `cols`, `rows`, `width` and `height`.
`toArray()` returns `['cols' => ..., 'rows' => ...]`.

## Raw mode

```php
$mode = $terminal->enableRawMode();
if ($mode === false) {
    throw new RuntimeException('Raw mode is unavailable.');
}

try {
    // Native input loop.
} finally {
    if (!$terminal->restoreMode()) {
        throw new RuntimeException('Could not restore terminal mode.');
    }
}
```

Repeated `enableRawMode()` calls on one session are idempotent and return the
active `ModeToken`. A successful restore consumes that token. The session also
attempts restoration during normal destruction, but explicit restoration is the
only way for an application to observe restore failure.

## Key input

`readKey(?float $timeout = null, ?float $sequenceTimeout = null)` returns:

- an `Io\Terminal\Key` case for recognized special keys
- a string for printable/control input that is not normalized to a named key
- `false` for timeout or unavailable native input

Named keys include arrows, Enter, Tab, Backspace, Escape, Home, End, Delete,
PageUp, PageDown, Resize and F1 through F12.

On POSIX, `sequenceTimeout` controls the wait between bytes of an escape or
UTF-8 sequence. Windows uses native console events and ignores that argument.

## Hidden input

`readSecret(string $prompt = ''): string` performs a silent native read. It
does not render masks or an automatic newline. The caller owns surrounding UI.

Ctrl+C, Ctrl+D and Escape cancel with `RuntimeException`. Operational failures
and failed mode restoration also use `RuntimeException`. Invalid arguments and
closed resources retain normal PHP `TypeError`/`ValueError` behavior.

## Enums and compatibility objects

`Backend`, `Stream`, `ColorDepth` and `Key` are unbacked enums. Use
`->name`, not `->value`. `ColorDepth::bits()` returns 0, 4, 8 or 24.

`ModeToken` is opaque and cannot be cloned or serialized.

## Legacy facade

The static `Terminal\Terminal` facade and `Terminal\*` aliases remain for
compatibility with pre-1.0 users. New integrations should use
`Io\Terminal\Terminal`. The legacy `getSize()` returns an array rather than
`TerminalSize`.

For exact signatures, see [`terminal.stub.php`](../terminal.stub.php).
