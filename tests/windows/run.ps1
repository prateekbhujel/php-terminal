$ErrorActionPreference = 'Stop'
$php = @(Get-ChildItem "$PSScriptRoot/../../build" -Filter php.exe -Recurse | Where-Object { $_.Directory.Name -eq 'php-bin' })
$dll = @(Get-ChildItem "$PSScriptRoot/../../build" -Filter php_terminal.dll -Recurse | Where-Object { $_.FullName -match '\\x64\\Release' })
if ($php.Count -ne 1 -or $dll.Count -ne 1) {
    throw "Expected one freshly built PHP/DLL pair, got $($php.Count)/$($dll.Count)"
}
$harness = Join-Path $env:RUNNER_TEMP 'TerminalConsoleTests.exe'
$csc = Join-Path $env:WINDIR 'Microsoft.NET/Framework64/v4.0.30319/csc.exe'
$source = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot 'ConsoleHarness.cs')).Path
$fixture = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot 'console.php')).Path
& $csc /nologo /target:exe "/out:$harness" $source
if ($LASTEXITCODE -ne 0) { throw 'Console test compilation failed' }
& $harness $php[0].FullName $dll[0].FullName $fixture
if ($LASTEXITCODE -ne 0) { throw 'Native console tests failed' }
