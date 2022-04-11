#include <internal/scrlife.h>

#ifdef _TV_UNIX

#include <initializer_list>

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

char StderrRedirector::stderrBuffer alignas(4096) [4096];
const char StderrRedirector::truncationMsg[] = "\ntvision: Standard error truncated to 16 MiB.\n";

StderrRedirector::StderrRedirector() noexcept
{
    // Text written into standard error will mess up the display or simply
    // get lost since the application is using the alternate screen buffer,
    // with no scrollback.
    // Make 'stderr' point to a temporary file so that the contents can be
    // dumped to the screen after restoring the screen buffer.
    int flags;
    FILE *file;
    if ( fileno(stderr) == STDERR_FILENO
         && isatty(STDERR_FILENO)
         && (flags = fcntl(STDERR_FILENO, F_GETFD)) != -1
         && (file = tmpfile()) )
    {
        int ttyFd = dup(STDERR_FILENO);
        int fileFd = dup(fileno(file));
        if ( fclose(file) == 0
             && ttyFd != -1
             && fileFd != -1
             && dup2(fileFd, STDERR_FILENO) != -1 )
        {
            fcntl(STDERR_FILENO, F_SETFD, flags);
            fcntl(ttyFd, F_SETFD, flags | FD_CLOEXEC);
            fcntl(fileFd, F_SETFD, flags | FD_CLOEXEC);
            this->ttyFd = ttyFd;
            this->fileFd = fileFd;
        }
        else
            for (int fd : {fileFd, ttyFd})
                close(fd);
    }
}

static bool isSameFile(int fd1, int fd2)
{
    struct stat stat1, stat2;
    return fstat(fd1, &stat1) != -1
        && fstat(fd2, &stat2) != -1
        && stat1.st_dev == stat2.st_dev
        && stat1.st_ino == stat2.st_ino;
}

static void copyFile(int from, int to, off_t size, char (&buf)[4096])
{
    ssize_t r, w;
    size_t left = size;
    lseek(from, 0, SEEK_SET);
    while (left > 0 && (r = read(from, buf, min(left, sizeof(buf)))) > 0)
    {
        left -= (size_t) r;
        while (r > 0 && (w = write(to, buf, r)) > 0)
            r -= w;
    }
}

StderrRedirector::~StderrRedirector()
{
    // Restore standard error to the default state as long as it still
    // refers to the temporary file, then dump the temporary file to it.
    if (isSameFile(fileFd, STDERR_FILENO))
    {
        dup2(ttyFd, STDERR_FILENO);

        off_t size = lseek(fileFd, 0, SEEK_END);
        if (size != (off_t) -1)
        {
            bool truncated;
            if ((truncated = (size > maxStderrSize)))
            {
                ftruncate(fileFd, maxStderrSize);
                size = maxStderrSize;
            }

            copyFile(fileFd, ttyFd, size, stderrBuffer);

            if (truncated)
                write(ttyFd, truncationMsg, sizeof(truncationMsg) - 1);
        }
    }

    for (int fd : {fileFd, ttyFd})
        close(fd);
}

#endif // _TV_UNIX
