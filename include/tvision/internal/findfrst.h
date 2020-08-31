#ifndef FINDFRST_H
#define FINDFRST_H

#include <string>
#include <vector>
#include <mutex>
#include <filesystem>

#include <tvision/tv.h>
#include <dos.h>

// A class implementing the behaviour of findfirst and findnext.
// allocate() assigns a FindFirstRec to the provided find_t struct and sets
// the search filters. get() simply retrieves the FindFirstRec that was
// assigned to a find_t struct. next() performs the actual search and
// automatically updates the find_t struct.

class FindFirstRec
{
    using directory_iterator = std::filesystem::directory_iterator;
    using directory_entry = std::filesystem::directory_entry;
    using path = std::filesystem::path;

    struct find_t     *finfo;
    directory_iterator dirStream;
    unsigned           searchAttr;
    std::string        searchDir;
    std::string        wildcard;

public:

    static FindFirstRec* allocate(struct find_t *, unsigned int, const char *);
    static FindFirstRec* get(struct find_t *);

    bool next();

private:

    bool streamValid();
    bool open();
    void close();
    bool setParameters(unsigned int, const char *);
    bool setPath(const char*);
    bool matchEntry(const char *);

    bool attrMatch(unsigned int attrib);
    static bool wildcardMatch(char const *wildcard, char const *filename);
    static unsigned int cvtAttr(struct stat *st, const char* filename);
    static void cvtTime(struct stat *st, struct find_t *fileinfo);

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
        std::lock_guard<std::mutex> lock()
        {
            return std::lock_guard<std::mutex>(m);
        }
    } static recList;
};

#endif
