using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;

// Uses an actual Win32 console, with PHP's stdin redirected to an unrelated pipe.
// No mocking of extension functions or console mode operations.
class ConsoleHarness
{
    [StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode)]
    struct InputRecord
    {
        [FieldOffset(0)] public ushort Type;
        [FieldOffset(4)] public int KeyDown;
        [FieldOffset(8)] public ushort Repeat;
        [FieldOffset(10)] public ushort VirtualKey;
        [FieldOffset(12)] public ushort ScanCode;
        [FieldOffset(14)] public char Character;
        [FieldOffset(16)] public uint ControlState;
    }
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool AllocConsole();
    [DllImport("kernel32.dll")] static extern bool FreeConsole();
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr CreateFileW(string name, uint access, uint share, IntPtr security, uint creation, uint flags, IntPtr template);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool GetConsoleMode(IntPtr handle, out uint mode);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool SetConsoleMode(IntPtr handle, uint mode);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool WriteConsoleInputW(IntPtr handle, InputRecord[] records, uint count, out uint written);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool FlushConsoleInputBuffer(IntPtr handle);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr handle);

    static void Check(bool ok, string message)
    {
        if (!ok) throw new Exception(message + " (Win32 " + Marshal.GetLastWin32Error() + ")");
    }
    static uint Mode(IntPtr input)
    {
        uint mode;
        Check(GetConsoleMode(input, out mode), "GetConsoleMode");
        return mode;
    }
    static void Await(Func<bool> ready, string message)
    {
        var time = Stopwatch.StartNew();
        while (!ready())
        {
            if (time.ElapsedMilliseconds > 5000) throw new Exception(message);
            Thread.Sleep(5);
        }
    }
    static string Quote(string arg) { return "\"" + arg.Replace("\"", "\\\"") + "\""; }
    static string Run(string php, string dll, string script, IntPtr input, string scenario, string chars, ushort key, string expected, ushort repeat)
    {
        Check(FlushConsoleInputBuffer(input), "FlushConsoleInputBuffer");
        uint before = Mode(input) | 7; // echo, line and processed input
        Check(SetConsoleMode(input, before), "Set initial mode");
        var info = new ProcessStartInfo(php, "-n -d " + Quote("extension=" + dll) + " " + Quote(script) + " " + scenario);
        info.UseShellExecute = false;
        info.CreateNoWindow = false;
        info.RedirectStandardInput = info.RedirectStandardOutput = info.RedirectStandardError = true;
        using (var child = Process.Start(info))
        {
            try
            {
                var ready = child.StandardOutput.ReadLineAsync();
                Check(ready.Wait(5000) && ready.Result == "READY", "Child startup: " + scenario);
                Await(() => (Mode(input) & 7) == 0 || child.HasExited, "Child did not enter raw mode");
                Check(!child.HasExited, "Child exited before native input");
                var records = new InputRecord[chars.Length];
                for (int i = 0; i < chars.Length; i++)
                {
                    records[i].Type = 1;
                    records[i].KeyDown = 1;
                    records[i].Repeat = repeat;
                    records[i].Character = chars[i];
                    records[i].VirtualKey = key != 0 && i == chars.Length - 1 ? key : chars[i] == '\r' ? (ushort)13 : chars[i] == '\b' ? (ushort)8 : (ushort)0;
                }
                uint written;
                Check(WriteConsoleInputW(input, records, (uint)records.Length, out written) && written == records.Length, "Inject key events");
                string prefix = "";
                if (scenario == "raw")
                {
                    var raw = child.StandardOutput.ReadLineAsync();
                    Check(raw.Wait(5000) && raw.Result == "78|RAW", "Raw-mode handshake");
                    Check((Mode(input) & 7) == 0, "readKey restored mode during active raw session");
                    prefix = raw.Result + "\n";
                    child.StandardInput.WriteLine("restore");
                    child.StandardInput.Flush();
                }
                var output = child.StandardOutput.ReadToEndAsync();
                var error = child.StandardError.ReadToEndAsync();
                Check(child.WaitForExit(5000), "Child timed out");
                Check(child.ExitCode == 0 && error.Result == "", "Child failed: " + error.Result);
                Check(prefix + output.Result == expected, scenario + " output: " + prefix + output.Result);
                Check(Mode(input) == before, scenario + " did not restore mode");
                return scenario + ": passed";
            }
            finally
            {
                if (!child.HasExited) child.Kill();
                SetConsoleMode(input, before);
            }
        }
    }
    static int Main(string[] args)
    {
        // Preserve CI's redirected stdout while allocating an independent console.
        var log = Console.Out;
        FreeConsole();
        if (!AllocConsole()) { log.WriteLine("AllocConsole failed"); return 1; }
        IntPtr input = CreateFileW("CONIN$", 0xc0000000, 3, IntPtr.Zero, 3, 0, IntPtr.Zero);
        try
        {
            Check(input != new IntPtr(-1), "Open console input");
            log.WriteLine(Run(args[0], args[1], args[2], input, "key", "x", 0, "78", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key", "\0", 38, "Up", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key", "\ud83d\ude00", 0, "f09f9880", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "secret", "caf\u00e9\ud83d\ude00\b!\r", 0, "636166c3a921|70773a20", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "secret", "x\r", 0, "787878|70773a20", 3));
            foreach (char abort in new char[] { '\x03', '\x04', '\x1b' })
                log.WriteLine(Run(args[0], args[1], args[2], input, "abort", "bad" + abort, abort == '\x1b' ? (ushort)27 : (ushort)0, "cancelled|70773a20", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "raw", "x", 0, "78|RAW\nrestored", 1));
            return 0;
        }
        catch (Exception e) { log.WriteLine(e.ToString()); return 1; }
        finally { CloseHandle(input); FreeConsole(); }
    }
}
