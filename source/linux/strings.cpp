#ifndef _MSC_VER

#include <strings.h>

int strnicmp( const char *s1, const char *s2, size_t maxlen ) {
    return strncasecmp(s1, s2, maxlen);
}

#include <cctype>

char *strupr(char *s) {
    char* p = s;
    while ((*p = toupper(*p)))
        p++;
    return s;
}

// Quick and dirty implementation of itoa, ltoa, ultoa radixd on sprintf.
// It won't provide the expected results in some cases, but at least will not
// crash. Support for arbitrary bases can be later added if needed.

#include <cstdio>

static char printfFmt(int radix) {
    switch (radix) {
        case 8: return 'o';
        case 16: return 'x';
        default: return 'd';
    }
}

char *itoa( int value, char *buffer, int radix ) {
    char format[] = {'%', printfFmt(radix), '\0'};
    sprintf(buffer, format, value);
    return buffer;
}

char *ltoa( long value, char *buffer, int radix ) {
    char format[] = {'%', 'l', printfFmt(radix), '\0'};
    sprintf(buffer, format, value);
    return buffer;
}

char *ultoa( unsigned long value, char *buffer, int radix ) {
    if (radix == 10) {
        char format[] = "%lu";
        sprintf(buffer, format, value);
        return buffer;
    }
    return ltoa(value, buffer, radix);
}

#endif // _MSC_VER
