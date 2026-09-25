using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
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
        [FieldOffset(4)] public short BufferCols;
        [FieldOffset(6)] public short BufferRows;
        [FieldOffset(4)] public short MouseX;
        [FieldOffset(6)] public short MouseY;
        [FieldOffset(8)] public uint ButtonState;
        [FieldOffset(12)] public uint MouseControlState;
        [FieldOffset(16)] public uint MouseEventFlags;
        [FieldOffset(4)] public int Focused;
        [FieldOffset(4)] public uint CommandId;
    }
    [StructLayout(LayoutKind.Sequential)]
    struct SecurityAttributes
    {
        public int Length;
        public IntPtr SecurityDescriptor;
        [MarshalAs(UnmanagedType.Bool)] public bool InheritHandle;
    }
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool AllocConsole();
    [DllImport("kernel32.dll")] static extern bool FreeConsole();
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr CreateFileW(string name, uint access, uint share, IntPtr security, uint creation, uint flags, IntPtr template);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true, EntryPoint = "CreateFileW")]
    static extern IntPtr CreateInheritableFileW(string name, uint access, uint share, ref SecurityAttributes security, uint creation, uint flags, IntPtr template);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool GetConsoleMode(IntPtr handle, out uint mode);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool SetConsoleMode(IntPtr handle, uint mode);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool WriteConsoleInputW(IntPtr handle, InputRecord[] records, uint count, out uint written);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool FlushConsoleInputBuffer(IntPtr handle);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)] static extern uint GetConsoleTitleW(StringBuilder title, uint size);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)] static extern bool SetConsoleTitleW(string title);
    [DllImport("kernel32.dll")] static extern IntPtr GetStdHandle(int handle);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool SetStdHandle(int handle, IntPtr value);
    [DllImport("kernel32.dll", SetLastError = true)] static extern bool CloseHandle(IntPtr handle);

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
    static string Title()
    {
        var title = new StringBuilder(65536);
        GetConsoleTitleW(title, (uint)title.Capacity);
        return title.ToString();
    }
    static string RunTitle(string php, string dll, string script, string scenario)
    {
        string before = Title();
        IntPtr stdout = GetStdHandle(-11);
        Check(stdout != IntPtr.Zero && stdout != new IntPtr(-1), "Get stdout");
        var security = new SecurityAttributes { Length = Marshal.SizeOf(typeof(SecurityAttributes)), InheritHandle = true };
        IntPtr output = CreateInheritableFileW("CONOUT$", 0xc0000000, 3, ref security, 3, 0, IntPtr.Zero);
        Check(output != new IntPtr(-1), "Open console output");
        try
        {
            // The title's native path needs console stdout. Keep test messages on stderr.
            Check(SetStdHandle(-11, output), "Set console stdout");
            var info = new ProcessStartInfo(php, "-n -d " + Quote("extension=" + dll) + " " + Quote(script) + " " + scenario);
            info.UseShellExecute = false;
            info.CreateNoWindow = false;
            info.RedirectStandardInput = info.RedirectStandardError = true;
            using (var child = Process.Start(info))
            {
                try
                {
                    string unicodeTitle = "Terminal caf\u00e9 \u65e5\u672c\u8a9e \ud83d\ude00";
                    string[] titles = { unicodeTitle, unicodeTitle, unicodeTitle, "" };
                    string[] results = { "true", "false", "false", "true" };
                    for (int i = 0; i < titles.Length; i++)
                    {
                        var result = child.StandardError.ReadLineAsync();
                        Check(result.Wait(5000) && result.Result == results[i], "Title result " + i);
                        Check(Title() == titles[i], "Console title mismatch " + i);
                        child.StandardInput.WriteLine("checked");
                        child.StandardInput.Flush();
                    }
                    var error = child.StandardError.ReadToEndAsync();
                    Check(child.WaitForExit(5000), "Title child timed out");
                    Check(child.ExitCode == 0 && error.Result == "", "Title child failed: " + error.Result);
                }
                finally
                {
                    if (!child.HasExited) child.Kill();
                }
            }
            return scenario + ": passed";
        }
        finally
        {
            Check(SetStdHandle(-11, stdout), "Restore stdout");
            Check(CloseHandle(output), "Close console output");
            Check(SetConsoleTitleW(before), "Restore console title");
        }
    }
    static string Run(string php, string dll, string script, IntPtr input, string scenario, string chars, ushort key, string expected, ushort repeat, InputRecord[] injected = null)
    {
        Check(FlushConsoleInputBuffer(input), "FlushConsoleInputBuffer");
        uint before = Mode(input) | 7; // echo, line and processed input
        if (scenario == "event-raw") before &= ~8U; // verify raw mode enables window events
        Check(SetConsoleMode(input, before), "Set initial mode");
        var info = new ProcessStartInfo(php, "-n -d " + Quote("extension=" + dll) + " " + Quote(script) + " " + scenario);
        info.UseShellExecute = false;
        info.CreateNoWindow = false;
        info.RedirectStandardInput = info.RedirectStandardOutput = info.RedirectStandardError = true;
        info.RedirectStandardInput = scenario != "key-repeat-legacy";
        using (var child = Process.Start(info))
        {
            try
            {
                var ready = child.StandardOutput.ReadLineAsync();
                Check(ready.Wait(5000) && ready.Result == "READY", "Child startup: " + scenario);
                Await(() => (Mode(input) & 7) == 0 || child.HasExited, "Child did not enter raw mode");
                Check(!child.HasExited, "Child exited before native input");
                var records = injected ?? new InputRecord[chars.Length];
                for (int i = 0; injected == null && i < chars.Length; i++)
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
                if (scenario == "raw" || scenario == "event-raw" || scenario == "raw-overlap")
                {
                    var raw = child.StandardOutput.ReadLineAsync();
                    Check(raw.Wait(5000) && raw.Result == "78|RAW", "Raw-mode handshake");
                    Check((Mode(input) & 7) == 0, "readKey restored mode during active raw session");
                    if (scenario == "event-raw")
                    {
                        Check(Mode(input) == ((before & ~7U) | 8U), "Window events were not kept enabled in raw mode");
                        var resize = new InputRecord[] { new InputRecord { Type = 4, BufferCols = 101, BufferRows = 37 } };
                        uint resizeWritten;
                        Check(WriteConsoleInputW(input, resize, 1, out resizeWritten) && resizeWritten == 1, "Queue resize between reads");
                    }
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
            log.WriteLine(RunTitle(args[0], args[1], args[2], "title"));
            log.WriteLine(RunTitle(args[0], args[1], args[2], "title-resource"));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key", "x", 0, "78", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key", "\0", 38, "Up", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key", "\ud83d\ude00", 0, "f09f9880", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key-repeat", "x", 0, "78|78|78", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key-repeat-legacy", "x", 0, "78|78|78", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key-repeat", "\0", 38, "Up|Up|Up", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key-repeat", "\ud83d\ude00", 0, "f09f9880|f09f9880|f09f9880", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key-repeat-order", "xy", 0, "78|78|78|79|79|79", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key-secret", "x\r", 0, "78|7878", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "key-secret", "\ud83d\ude00\r", 0, "f09f9880|f09f9880f09f9880", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "secret", "caf\u00e9\ud83d\ude00\b!\r", 0, "636166c3a921|70773a20", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "secret", "x\r", 0, "787878|70773a20", 3));
            foreach (char abort in new char[] { '\x03', '\x04', '\x1b' })
                log.WriteLine(Run(args[0], args[1], args[2], input, "abort", "bad" + abort, abort == '\x1b' ? (ushort)27 : (ushort)0, "cancelled|70773a20", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "raw", "x", 0, "78|RAW\nrestored", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "raw-overlap", "x", 0, "78|RAW\nrestored", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-key", "", 0,
                "key|Left|-|1|2|37|75|0|8|1|0|0", 1,
                new InputRecord[] { new InputRecord { Type = 1, KeyDown = 1, Repeat = 2, VirtualKey = 37, ScanCode = 75, ControlState = 8 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-key", "", 0,
                "key|-|78|1|1|88|45|120|18|0|1|1", 1,
                new InputRecord[] { new InputRecord { Type = 1, KeyDown = 1, Repeat = 1, VirtualKey = 88, ScanCode = 45, Character = 'x', ControlState = 18 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-key", "", 0,
                "key|-|-|0|1|16|42|0|16|0|0|1", 1,
                new InputRecord[] { new InputRecord { Type = 1, KeyDown = 0, Repeat = 1, VirtualKey = 16, ScanCode = 42, ControlState = 16 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-key", "", 0,
                "key|-|-|1|1|17|29|0|8|1|0|0", 1,
                new InputRecord[] { new InputRecord { Type = 1, KeyDown = 1, Repeat = 1, VirtualKey = 17, ScanCode = 29, ControlState = 8 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-key", "", 0,
                "key|-|-|1|1|0|0|55357|0|0|0|0", 1,
                new InputRecord[] { new InputRecord { Type = 1, KeyDown = 1, Repeat = 1, Character = '\ud83d' } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-key", "", 0,
                "key|-|-|1|1|0|0|56832|0|0|0|0", 1,
                new InputRecord[] { new InputRecord { Type = 1, KeyDown = 1, Repeat = 1, Character = '\ude00' } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-surrogates", "\ud83d\ude00", 0,
                "55357:-|56832:-", 1));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-pending", "x", 0,
                "78|2|78|false", 3));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-resize", "", 0,
                "resize|101|37", 1,
                new InputRecord[] { new InputRecord { Type = 4, BufferCols = 101, BufferRows = 37 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-mouse", "", 0,
                "mouse|12|9|1|8|0", 1,
                new InputRecord[] { new InputRecord { Type = 2, MouseX = 12, MouseY = 9, ButtonState = 1, MouseControlState = 8, MouseEventFlags = 0 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-focus", "", 0,
                "focus|1", 1,
                new InputRecord[] { new InputRecord { Type = 16, Focused = 1 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-menu", "", 0,
                "menu|42", 1,
                new InputRecord[] { new InputRecord { Type = 8, CommandId = 42 } }));
            log.WriteLine(Run(args[0], args[1], args[2], input, "event-raw", "x", 0,
                "78|RAW\nresize|101|37|restored", 1));
            return 0;
        }
        catch (Exception e) { log.WriteLine(e.ToString()); return 1; }
        finally { CloseHandle(input); FreeConsole(); }
    }
}
