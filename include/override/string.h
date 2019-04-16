#ifndef STRING_H
#define STRING_H

// Redirect to <cstring>

#include <cstring>

// Workaround for obsolete functions previously in string.h

#include <cstdio>
#include <strings.h> // Requires POSIX, sorry
#define strnicmp strncasecmp

static const char * radix2cnv(int radix) {
  switch (radix) {
    case 8: return "o";
    case 10: return "d";
    case 16: return "x";
  }
  return "";
}

static char * ltoa(long l, char * buffer, int radix) {
  char format[5] = "%l";
  strcat(format, radix2cnv(radix));
  sprintf(buffer, format, l);
  return buffer;
}

static char * itoa(int i, char * buffer, int radix) {
  char format[5] = "%";
  strcat(format, radix2cnv(radix));
  sprintf(buffer, format, i);
  return buffer;
}

#include <cctype>

// Non-Unicode-compatible draft.
// http://www.cplusplus.com/forum/general/21215/#msg111074
static char * strupr(char * s) {
  char* p = s;
    while (*p = toupper(*p))
      p++;
  return s;
}

#else

#include_next <string.h>

#endif
