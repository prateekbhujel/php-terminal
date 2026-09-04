--TEST--
Terminal\Terminal::supportsAnsi honors CLICOLOR_FORCE and NO_COLOR precedence
--EXTENSIONS--
terminal
--FILE--
<?php
$extension = dirname(__DIR__) . '/modules/terminal.' . PHP_SHLIB_SUFFIX;

function check_ansi(array $env): bool
{
    global $extension;
    $cmd = escapeshellarg(PHP_BINARY) . ' -n -d extension=' . escapeshellarg($extension) . ' -r ' . escapeshellarg('var_dump(Terminal\Terminal::supportsAnsi(Terminal\Stream::Stdout));');
    $descriptors = [
        0 => ['pipe', 'r'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
    ];

    // Merge clean base env
    $cleanEnv = [
        'PATH' => getenv('PATH'),
        'TERM' => 'dumb',
    ];
    foreach ($env as $k => $v) {
        if ($v === null) {
            unset($cleanEnv[$k]);
        } else {
            $cleanEnv[$k] = $v;
        }
    }

    $proc = proc_open($cmd, $descriptors, $pipes, null, $cleanEnv);
    if (!is_resource($proc)) {
        return false;
    }

    fclose($pipes[0]);
    $out = trim(stream_get_contents($pipes[1]));
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($proc);

    return $out === 'bool(true)';
}

// 1. Piped stream with CLICOLOR_FORCE=1 should enable ANSI
var_dump(check_ansi(['CLICOLOR_FORCE' => '1']));

// 2. NO_COLOR takes precedence over CLICOLOR_FORCE
var_dump(check_ansi(['CLICOLOR_FORCE' => '1', 'NO_COLOR' => '1']));

// 3. CLICOLOR_FORCE=0 does not force ANSI
var_dump(check_ansi(['CLICOLOR_FORCE' => '0']));

// 4. CLICOLOR_FORCE empty does not force ANSI
var_dump(check_ansi(['CLICOLOR_FORCE' => '']));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
