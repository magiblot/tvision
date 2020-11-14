#ifndef FINDFRST_H
#define FINDFRST_H

#include <tvision/tv.h>
#include <string>
#include <vector>
#include <mutex>
#include <dos.h>

#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

// A class implementing the behaviour of findfirst and findnext.
// allocate() assigns a FindFirstRec to the provided find_t struct and sets
// the search filters. get() simply retrieves the FindFirstRec that was
// assigned to a find_t struct. next() performs the actual search and
// automatically updates the find_t struct.

class FindFirstRec
{

public:

    static FindFirstRec* allocate(struct find_t *, unsigned, const char *);
    static FindFirstRec* get(struct find_t *);

    bool next();

private:

    struct find_t     *finfo;
    unsigned           searchAttr;

#ifndef _WIN32
    DIR               *dirStream {0};
    std::string        searchDir;
    std::string        wildcard;
#else
    HANDLE             hFindFile {INVALID_HANDLE_VALUE};
    std::string        fileName;
#endif // _WIN32

    bool open();
    void close();
    bool setParameters(unsigned, const char *);
    bool attrMatch(unsigned attrib);

#ifndef _WIN32
    bool setPath(const char*);
    bool matchEntry(struct dirent*);

    static bool wildcardMatch(char const *wildcard, char const *filename);
    unsigned cvtAttr(const struct stat *st, const char* filename);
    static void cvtTime(const struct stat *st, struct find_t *fileinfo);
#else
    unsigned cvtAttr(const WIN32_FIND_DATAW *findData, const wchar_t* filename);
    static void cvtTime(const WIN32_FIND_DATAW *findData, struct find_t *fileinfo);
#endif // _WIN32

    // A vector of FindFirstRec that deallocates all directory streams
    // on destruction.
    class RecList : std::vector<FindFirstRec>
    {
        friend class FindFirstRec;
        using std::vector<FindFirstRec>::vector;
        ~RecList()
        {
            for (FindFirstRec &r : *this)
                r.close();
        }
        std::mutex m;
        operator std::mutex&()
        {
            return m;
        }
    } static recList;
};

#endif // FINDFRST_H
