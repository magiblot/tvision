#ifndef STRING_H
#define STRING_H

#include <cstring>
#include <cstdio>

// Workaround for obsolete functions previously in string.h

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

char * ltoa(long l, char * buffer, int radix) {
  char format[5] = "%l";
  strcat(format, radix2cnv(radix));
  sprintf(buffer, format, l);
  return buffer;
}

char * itoa(int i, char * buffer, int radix) {
  char format[5] = "%";
  strcat(format, radix2cnv(radix));
  sprintf(buffer, format, i);
  return buffer;
}

#else

#include_next <string.h>

#endif
