#include <internal/findfrst.h>
#include <internal/pathconv.h>
#include <system_error>
#include <sys/stat.h>

#define SPECIAL_BITS (_A_SUBDIR|_A_HIDDEN|_A_SYSTEM)

FindFirstRec::RecList FindFirstRec::recList;

FindFirstRec* FindFirstRec::allocate(struct find_t *fileinfo, unsigned attrib,
                                     const char *pathname)
{
    // The findfirst interface based on DOS call 0x4E doesn't provide a
    // findclose function. The strategy here is the same as in Borland's RTL:
    // new directory streams are allocated unless fileinfo is an address
    // that has already been passed to us before.
    if (!fileinfo)
        return 0;
    auto lock{recList.lock()};
    FindFirstRec *r = 0;
    size_t index;

    for (index = 0; index < recList.size(); ++index)
        if (recList[index].finfo == fileinfo)
        {
            r = &recList[index];
            break;
        }
    // At this point, 'index' is either the position of the matching FindFirstRec
    // item or the size of recList, in which case a new item will be added
    // at the end of the list and 'index' will be a valid index pointing at it.
    if (r)
        r->close();
    else
    {
        recList.emplace_back();
        r = &recList[index];
        r->finfo = fileinfo;
    }
    // If pathname is a valid directory, make fileinfo point to the allocated
    // FindFirstRec. Otherwise, return NULL.
    if (r->setParameters(attrib, pathname))
    {
        fileinfo->reserved = (long) index;
        return r;
    }
    else
        return 0;
}

FindFirstRec* FindFirstRec::get(struct find_t *fileinfo)
{
    // Return the FindFirstRec instance pointed to by fileinfo.
    auto lock{recList.lock()};
    size_t pos = fileinfo->reserved;
    if (0 <= pos && pos < recList.size() && recList[pos].finfo == fileinfo)
        return &recList[pos];
    return 0;
}

bool FindFirstRec::streamValid()
{
    return dirStream != directory_iterator();
}

bool FindFirstRec::setParameters(unsigned int attrib, const char *pathname)
{
    if (!streamValid())
    {
        searchAttr = attrib;
        if (setPath(pathname))
            return open();
    }
    return false;
}

bool FindFirstRec::next()
{
    while (streamValid())
    {
        const auto &&pathName = dirStream->path().filename();
        ++dirStream;
        if (matchEntry((const char *) pathName.u8string().c_str()))
            return true;
    }
    return false;
}

bool FindFirstRec::open()
{
    if (!streamValid())
    {
        std::error_code ec;
        dirStream = {searchDir, ec};
        return !ec && streamValid();
    }
    return false;
}

void FindFirstRec::close()
{
    dirStream = {};
}

bool FindFirstRec::setPath(const char* pathname)
{
    // Reject NULL or empty pathnames.
    if (pathname && *pathname)
    {
        searchDir = pathname;
#ifdef _WIN32
        // Convert directory separators temporary, but keep the drive letter.
        path_dos2unix(searchDir, false);
#else
        path_dos2unix(searchDir);
#endif
        // Win32's FindFirst is designed to reject paths ending with a
        // separator. But legacy code unaware of Unix separators may be unable
        // to remove it and call findfirst with such a pathname. Therefore,
        // we handle this case mercifully.
        if (searchDir.back() == '/')
            wildcard = '.';
        else
        {
            auto lastSlash = searchDir.find_last_of('/');
            // When pathname doesn't contain a '/', wildcard keeps the whole
            // string and searchDir is set to the current directory.
            wildcard = searchDir.substr(lastSlash + 1);
            if (lastSlash == std::string::npos)
                searchDir = "./";
            else
                searchDir = searchDir.substr(0, lastSlash + 1);
            // '*.*' stands for 'any name, any extension'. We shouldn't expect
            // a dot in the filename in this case. In Borland C++, '*' yields
            // the same result as '*.*' in 32-bit builds, while it only returns
            // extensionless files in 16-bit builds.
            if (wildcard == "*.*")
                wildcard = '*';
        }
        // At this point, searchDir always ends with a '/'.
#ifdef _WIN32
        path_unix2dos(searchDir);
#endif
        return true;
    }
    return false;
}

bool FindFirstRec::matchEntry(const char *name)
{
    struct stat st;
    if (wildcardMatch(wildcard.c_str(), name) &&
        stat((searchDir + name).c_str(), &st) == 0)
    {
        unsigned int fileAttr = cvtAttr(&st, name);
        if (attrMatch(fileAttr))
        {
            // Match found, fill finfo.
            finfo->size = st.st_size;
            finfo->attrib = fileAttr;
            cvtTime(&st, finfo);
            strnzcpy(finfo->name, name, sizeof(find_t::name));
            return true;
        }
    }
    return false;
}

bool FindFirstRec::attrMatch(unsigned int attrib)
{
    // Behaviour from the original _dos_findnext: 'if requested attribute
    // word includes hidden, system, or subdirectory bits, return
    // normal files AND those with any of the requested attributes'.
    return !(attrib & SPECIAL_BITS) || (searchAttr & attrib & SPECIAL_BITS);
}

bool FindFirstRec::wildcardMatch(char const *wildcard, char const *filename)
{
    // https://stackoverflow.com/a/3300547
    for (; *wildcard != '\0'; ++wildcard)
        switch (*wildcard)
        {
            case '?':
                if (*filename == '\0')
                    return false;
                ++filename;
                break;
            case '*':
                if (wildcard[1] == '\0')
                    return true;
                for (size_t i = 0; filename[i] != '\0'; ++i)
                    if (wildcardMatch(wildcard + 1, &filename[i]))
                        return true;
                return false;
            default:
                if (*filename != *wildcard)
                    return false;
                ++filename;
        }
    return *filename == '\0';
}

unsigned int FindFirstRec::cvtAttr(struct stat *st, const char* filename)
{
    // Returns file attributes in find_t format.
    unsigned int attr = 0; // _A_NORMAL
    if (filename[0] == '.')
        attr |= _A_HIDDEN;
    if (st->st_mode & S_IFDIR)
        attr |= _A_SUBDIR;
    else if (!(st->st_mode & S_IFREG)) // If not a regular file
        attr |= _A_SYSTEM;
#ifdef __unix__
    else if (!(st->st_mode & S_IWUSR)) // If no write access, innacurate.
        attr |= _A_RDONLY;
#endif
    return attr;
}

void FindFirstRec::cvtTime(struct stat *st, struct find_t *fileinfo)
{
    // Updates fileinfo with the times in st.
    struct FatDate {
        ushort  day     : 5, // Day of the month (1–31)
                month   : 4, // Month (1-12)
                year    : 7; // Year-1980
    } *wr_date = (FatDate *) &fileinfo->wr_date;
    struct FatTime {
        ushort  sec     : 5, // Seconds divided by 2
                min     : 6, // Minutes (0-59)
                hour    : 5; // Hour (0-23)
    } *wr_time = (FatTime *) &fileinfo->wr_time;

    struct tm *lt = localtime(&st->st_mtime);
    *wr_date = { ushort (lt->tm_mday),
                 ushort (lt->tm_mon + 1),
                 ushort (lt->tm_year - 80) };
    *wr_time = { ushort (lt->tm_sec/2),
                 ushort (lt->tm_min),
                 ushort (lt->tm_hour) };
}
