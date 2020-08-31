#ifndef PATHCONV_H
#define PATHCONV_H

#include <string>
#include <algorithm>
#include <string.h>
#include <cctype>

// path_dos2unix: replaces '\' with '/' and removes drive letter.

inline void path_dos2unix(std::string &s, bool drive=true) {
    std::replace(s.begin(), s.end(), '\\', '/');
    if (drive && s.size() > 1 && s[1] == ':' && isalpha(s[0]))
        s = s.substr(2);
}

inline void path_dos2unix(char *c, bool drive=true) {
    char *d = c;
    while ((d = strchr(d, '\\')))
        *d = '/';
    if (drive && *c && c[1] == ':' && isalpha(*c))
        memmove(c, c+2, strlen(c)-1); // Copies null terminator as well.
}

// path_unix2dos: replaces '/' with '\'.

inline void path_unix2dos(std::string &s) {
    std::replace(s.begin(), s.end(), '/', '\\');
}

inline void path_unix2dos(char *c) {
    while ((c = strchr(c, '/')))
        *c = '\\';
}
#endif
