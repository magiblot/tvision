#include <internal/unixclip.h>

#ifdef _TV_UNIX

#include <internal/getenv.h>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <poll.h>

namespace tvision
{

enum { clipboardTimeoutMs = 1500 };

struct Command
{
    const char * const *argv;
    const char *requiredEnv;
    const char * const *customEnv;
};

#ifdef __APPLE__
constexpr const char * pbcopyArgv[] = {"pbcopy", 0};
constexpr const char * pbpasteArgv[] = {"pbpaste", 0};
#else
constexpr const char * wlCopyArgv[] = {"wl-copy", 0};
constexpr const char * xselCopyArgv[] = {"xsel", "--input", "--clipboard", 0};
constexpr const char * xclipCopyArgv[] = {"xclip", "-in", "-selection", "clipboard", 0};
constexpr const char * wslCopyArgv[] = {"/mnt/c/Windows/System32/cmd.exe", "/D", "/Q", "/C", "clip.exe", 0};
constexpr const char * wlPasteArgv[] = {"wl-paste", "--no-newline", 0};
constexpr const char * xselPasteArgv[] = {"xsel", "--output", "--clipboard", 0};
constexpr const char * xclipPasteArgv[] = {"xclip", "-out", "-selection", "clipboard", 0};
constexpr const char * wslPasteArgv[] =
{
    // PowerShell is the only native tool which allows reading the clipboard.
    // It must be launched in a separate window to work around
    // https://github.com/microsoft/terminal/issues/280. Because of this,
    // a temporary file is required to read PowerShell's output.
    // In addition, an environment variable is used to insert quoutes in order
    // to work around https://github.com/microsoft/WSL/issues/2835.
    // Also, since the PATH variable might get altered when using sudo, it is
    // best to use the absolute path of cmd.exe.
    "/mnt/c/Windows/System32/cmd.exe", "/D", "/Q", "/C",
    "FOR",
        "%i", "IN", "(", "%q%%TMP%%q%", "%q%%TEMP%%q%", "%q%%USERPROFILE%%q%", "\\.", ".", ")",
    "DO",
        "set", "TMPNAM=%q%%~i\\tmp%RANDOM%%RANDOM%.tmp%q%", "&&",
        "call", "copy", "/Y", "NUL", "^%TMPNAM^%", ">", "NUL", "&&",
        "(", "cmd", "/C", "start", "/I", "/MIN", "/WAIT",
            "powershell", "-NoProfile", "-NoLogo", "-NonInteractive", "-WindowStyle", "Hidden",
                "Get-Clipboard", "-Raw", "^^^|",
                "Out-File", "-NoNewline", "-Encoding", "unicode", "-FilePath", "'^%TMPNAM^%'", "^&^&",
            "type", "^%TMPNAM^%", "^&",
            "del", "/Q", "^%TMPNAM^%", "^>", "NUL", ")", "&&",
        "exit",
    0,
};
constexpr const char *wslPasteEnv[] =
{
    "WSLENV", "q",
    "q", "\"",
    0,
};
#endif

constexpr Command copyCommands[] =
{
#ifdef __APPLE__
    {pbcopyArgv},
#else
    {wslCopyArgv},
    {wlCopyArgv, "WAYLAND_DISPLAY"},
    {xselCopyArgv, "DISPLAY"},
    {xclipCopyArgv, "DISPLAY"},
#endif
};

constexpr Command pasteCommands[] =
{
#ifdef __APPLE__
    {pbpasteArgv},
#else
    {wlPasteArgv, "WAYLAND_DISPLAY"},
    {xselPasteArgv, "DISPLAY"},
    {xclipPasteArgv, "DISPLAY"},
    {wslPasteArgv, nullptr, wslPasteEnv},
#endif
};

static bool executable_exists(const char *name);
static TSpan<char> read_subprocess(const char * const cmd[], const char * const env[], int timeoutMs);
static bool write_subprocess(const char * const cmd[], TStringView, int timeoutMs);

static bool commandIsAvailable(const Command &cmd)
{
    return (!cmd.requiredEnv || !getEnv<TStringView>(cmd.requiredEnv).empty()) && executable_exists(cmd.argv[0]);
}

bool UnixClipboard::setClipboardText(TStringView text) noexcept
{
    for (auto &cmd : copyCommands)
        if (commandIsAvailable(cmd))
        {
            if (write_subprocess(cmd.argv, text, clipboardTimeoutMs))
                return true;
            break;
        }
    return false;
}

bool UnixClipboard::requestClipboardText(void (&accept)(TStringView)) noexcept
{
    for (auto &cmd : pasteCommands)
        if (commandIsAvailable(cmd))
        {
            TSpan<char> text = read_subprocess(cmd.argv, cmd.customEnv, clipboardTimeoutMs);
            if (text.data())
            {
                accept(text);
                free(text.data());
                return true;
            }
            break;
        }
    return false;
}

static bool executable_exists(const char *name)
{
    const char *path = "";
    if (name[0] != '/' && !(path = getenv("PATH")))
        path = "/usr/local/bin:/bin:/usr/bin";
    const char *end = path + strlen(path);
    size_t nameLen = strlen(name);
    const char *p = path;
    do
    {
        char buf[PATH_MAX];
        const char *q = strchr(p, ':');
        if (!q)
            q = end;
        if (q - p + nameLen + 2 <= PATH_MAX)
        {
            memcpy(&buf[0], p, q - p);
            buf[q - p] = '/';
            memcpy(&buf[q - p + 1], name, nameLen);
            buf[q - p + nameLen + 1] = '\0';

            struct stat st;
            if (stat(buf, &st) == 0 && !(~st.st_mode & (S_IFREG | S_IXOTH)))
                return true;
        }
        p = q;
    }
    while (p < end && *p++);
    return false;
}

enum class run_subprocess_mode
{
    read,
    write,
};

struct run_subprocess_t
{
    pid_t pid {-1};
    int fd {-1};
};

static run_subprocess_t run_subprocess(const char * const argv[], const char * const env[], run_subprocess_mode mode)
{
    int fds[2];
    if (pipe(fds) == -1)
        return {};
    pid_t pid = fork();
    if (pid == 0)
    {
        for (auto *p = env; p && *p; p += 2)
            setenv(p[0], p[1], true);

        int nul     = open("/dev/null", O_RDWR);
        int new_in  = mode == run_subprocess_mode::read ? nul    : fds[0];
        int new_out = mode == run_subprocess_mode::read ? fds[1] : nul;

        if ( nul != -1
             && dup2(new_in, STDIN_FILENO) != -1
             && dup2(new_out, STDOUT_FILENO) != -1
             && dup2(nul, STDERR_FILENO) != -1
             && close(fds[0]) != -1
             && close(fds[1]) != -1
             && close(nul) != -1 )
        {
            execvp(argv[0], (char * const *) argv);
        }
        _Exit(1);
    }
    else if (pid > 0)
    {
        int pipe_end = mode == run_subprocess_mode::read ? 0 : 1;
        close(fds[1 - pipe_end]);
        return {pid, fds[pipe_end]};
    }
    for (int fd : fds)
        close(fd);
    return {};
}

static bool close_subprocess(run_subprocess_t &process)
{
    close(process.fd);
    int status;
    int res = waitpid(process.pid, &status, 0);
    return res > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

struct read_pipe_t
{
    char *data {nullptr};
    size_t size {0};
    bool incomplete {false};
};

static read_pipe_t read_pipe(int fd, int timeoutMs)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return {};
    enum { minReadSize = 4096 };
    char *buf = (char *) malloc(minReadSize);
    if (!buf)
        return {};
    size_t bytesRead = 0;
    size_t capacity = minReadSize;
    int res = -1;
    while (true)
    {
        ssize_t r;
        while ((r = read(fd, buf + bytesRead, capacity - bytesRead)) > 0)
        {
            bytesRead += (size_t) r;
            if (capacity - bytesRead < minReadSize)
            {
                if (void *tmp = realloc(buf, capacity *= 2))
                    buf = (char *) tmp;
                else
                {
                    free(buf);
                    return {nullptr, 0, true};
                }
            }
        }
        if (r == 0 || (r == -1 && errno != EAGAIN))
            break;
        struct pollfd pfd {fd, POLLIN};
        if ((res = poll(&pfd, 1, timeoutMs)) <= 0 || !(pfd.revents & POLLIN))
            break;
    }
    return {buf, bytesRead, res == 0};
}

struct write_pipe_t
{
    bool success {false};
    bool incomplete {false};
};

static write_pipe_t write_pipe(int fd, const char *data, size_t size, int timeoutMs)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return {};
    size_t bytesWritten = 0;
    int res = -1;
    while (true)
    {
        ssize_t r;
        while ((r = write(fd, data + bytesWritten, size - bytesWritten)) > 0)
            bytesWritten += (size_t) r;
        if (r == 0 || (r == -1 && errno != EAGAIN))
            break;
        struct pollfd pfd {fd, POLLOUT};
        if ((res = poll(&pfd, 1, timeoutMs)) <= 0 || !(pfd.revents & POLLOUT))
            break;
    }
    return {bytesWritten == size, res == 0};
}

static TSpan<char> read_subprocess(const char * const cmd[], const char * const env[], int timeoutMs)
{
    auto process = run_subprocess(cmd, env, run_subprocess_mode::read);
    if (process.pid == -1)
        return {};
    auto res = read_pipe(process.fd, timeoutMs);
    if (res.incomplete)
        kill(process.pid, SIGKILL);
    bool processOk = close_subprocess(process);
    if (processOk || res.size > 0)
        return {res.data, res.size};
    free(res.data);
    return {};
}

static bool write_subprocess(const char * const cmd[], TStringView text, int timeoutMs)
{
    auto process = run_subprocess(cmd, nullptr, run_subprocess_mode::write);
    if (process.pid == -1)
        return false;
    auto res = write_pipe(process.fd, text.data(), text.size(), timeoutMs);
    if (res.incomplete)
        kill(process.pid, SIGKILL);
    bool processOk = close_subprocess(process);
    return processOk && res.success;
}

} // namespace tvision

#endif // _TV_UNIX
