// Replacements for functions in Borland's C/C++ Run Time Library.
// The code below, although inspired by RTL version 1.5 from BC++ 4.52,
// has been written from scratch.

#include <internal/findfrst.h>
#include <internal/pathconv.h>
#include <dir.h>

#ifdef _WIN32
#include <bitset>
#endif

unsigned _dos_findfirst( const char *pathname, unsigned attrib,
                         struct find_t *fileinfo )
{
    // The original findfirst sets errno on failure. We don't do this for now.
    FindFirstRec *r;
    if ((r = FindFirstRec::allocate(fileinfo, attrib, pathname)))
        return r->next() ? 0 : -1;
    return -1;
}

unsigned _dos_findnext(struct find_t *fileinfo)
{
    FindFirstRec *r;
    if ((r = FindFirstRec::get(fileinfo)) && r->next())
        return 0;
    return -1;
}

int findfirst(const char *pathname, struct ffblk *ffblk, int attrib)
{
    return _dos_findfirst(pathname, attrib, (struct find_t *) ffblk);
}

int findnext(struct ffblk *ffblk)
{
    return _dos_findnext((struct find_t *) ffblk);
}

void fnmerge( char *pathP, const char *driveP, const char *dirP,
              const char *nameP, const char *extP )
{
    // fnmerge is often used before accessing files, so producing a
    // UNIX-formatted path fixes most cases of filesystem access.
    memset(pathP, 0, MAXPATH);
    size_t n = 0;
#ifdef _WIN32
    if (driveP && *driveP)
    {
        n += strnzcpy(&pathP[n], driveP, MAXPATH - n);
        if (pathP[n-1] != ':')
            n += strnzcpy(&pathP[n], ":", MAXPATH - n);
    }
#endif
    if (dirP && *dirP)
    {
        n += strnzcpy(&pathP[n], dirP, MAXPATH - n);
        if (pathP[n-1] != '\\' && pathP[n-1] != '/')
            n += strnzcpy(&pathP[n], "\\", MAXPATH - n);
    }
    if (nameP && *nameP)
        n += strnzcpy(&pathP[n], nameP, MAXPATH - n);
    if (extP && *extP)
    {
        if (*extP != '.')
            n += strnzcpy(&pathP[n], ".", MAXPATH - n);
        n += strnzcpy(&pathP[n], extP, MAXPATH - n);
    }
#ifdef _TV_UNIX
    path_dos2unix(pathP);
#endif
}

static bool CopyComponent(char* dstP, const char* startP, const char* endP, size_t MAX)
{
    // Copies a path component of length endP-startP into dstP.
    // If the component is empty, returns false and does nothing.
    if (endP != startP)
    {
        if (dstP)
            // This always adds a trailing '\0', thus the +1.
            strnzcpy(dstP, startP, std::min<size_t>(MAX, endP-startP+1));
        return true;
    }
    return false;
}

int fnsplit(const char *pathP, char *driveP, char *dirP, char *nameP, char *extP)
{
    int flags = 0;
    if (driveP)
        memset(driveP, 0, MAXDRIVE);
    if (dirP)
        memset(dirP, 0, MAXDIR);
    if (nameP)
        memset(nameP, 0, MAXFILE);
    if (extP)
        memset(extP, 0, MAXEXT);
    if (pathP && *pathP)
    {
        size_t len = strlen(pathP);
        const char *pathEnd = pathP+len;
        const char *caretP = 0;
        const char *slashP = 0; // Rightmost slash
        const char *lastDotP = 0; // Last dot in filename
        const char *firstDotP = 0; // First dot in filename
        for (size_t i = len - 1; i < len; --i)
            switch (pathP[i])
            {
                case '?':
                case '*':
                    // Wildcards are only detected in filename or extension.
                    if (!slashP)
                        flags |= WILDCARDS;
                    break;
                case '.':
                    if (!slashP)
                    {
                        if (!lastDotP)
                            lastDotP = pathP+i;
                        firstDotP = pathP+i;
                    }
                    break;
                case '\\':
                case '/':
                    if (!slashP)
                        slashP = pathP+i;
                    break;
                case ':':
                    if (i == 1)
                    {
                        caretP = pathP+i;
                        i = 0; // Exit loop, we don't check the drive letter.
                    }
                default:
                    ;
            }
        // These variables point after the last character of each component.
        const char *driveEnd = caretP ? caretP+1 : pathP;
        const char *dirEnd = slashP ? slashP+1 : driveEnd;
        const char *nameEnd = lastDotP ? lastDotP : pathEnd;
        // Special case: path ends with '.' or '..', thus there's no filename.
        if (lastDotP == pathEnd-1 && lastDotP - firstDotP < 2 && firstDotP == dirEnd)
            dirEnd = nameEnd = pathEnd;
        // Copy components and set flags.
        CopyComponent(driveP, pathP, driveEnd, MAXDRIVE) && (flags |= DRIVE);
        CopyComponent(dirP, driveEnd, dirEnd, MAXDIR) && (flags |= DIRECTORY);
        CopyComponent(nameP, dirEnd, nameEnd, MAXFILE) && (flags |= FILENAME);
        CopyComponent(extP, nameEnd, pathEnd, MAXEXT) && (flags |= EXTENSION);
    }
    return flags;
}

int getdisk() {
#ifdef _WIN32
    return _getdrive() - 1;
#else
    // Emulate drive C.
    return 'C' - 'A';
#endif
}

int setdisk(int drive)
{
#ifdef _WIN32
    _chdrive(drive - 1);
    return std::bitset<8*sizeof(ulong)>(_getdrives()).count();
#else
    return drive == getdisk() ? 0 : -1;
#endif
}

int getcurdir(int drive, char *direc)
{
    // direc is an array of length MAXDIR where the null-terminated directory
    // name will be placed, without drive specification nor leading backslash.
    // Note that drive 0 is the 'default' drive, 1 is drive A, etc.
    if (drive == 0 || drive-1 == getdisk())
    {
#ifdef _WIN32
        constexpr int lead = 3;
#else
        constexpr int lead = 1;
#endif
        char buf[MAXDIR+lead];
        if (getcwd(buf, MAXDIR+lead))
        {
            path_unix2dos(buf);
            strnzcpy(direc, buf+lead, MAXDIR);
            return 0;
        }
    }
    return -1;
}
