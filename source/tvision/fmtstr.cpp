/*------------------------------------------------------------*/
/* filename -       fmtstr.cpp                                */
/*                                                            */
/* function(s)                                                */
/*            formatStr                                       */
/*            vFormatStr                                      */
/*------------------------------------------------------------*/

#include <tvision/tv.h>

#include <stdio.h>

#if defined( __BORLANDC__ )
#define va_copy(dst, src) ((void) ((dst) = (src)))
#endif // __BORLANDC__

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  formatStr                                                             */
/*                                                                        */
/*  arguments:                                                            */
/*                                                                        */
/*      format  - string with format specifiers                           */
/*                                                                        */
/*      ...     - list of parameters                                      */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      A string formatted using vsnprintf, allocated dynamically.        */
/*      If vsnprintf fails, returns a copy of 'format'.                   */
/*      The returned string can be disposed of using 'delete[]'.          */
/*                                                                        */
/*------------------------------------------------------------------------*/

char *formatStr(const char _FAR *format, ...) noexcept
{
    va_list ap;

    va_start(ap, format);
    char *ret = vFormatStr(format, ap);
    va_end(ap);

    return ret;
}

char *vFormatStr(const char _FAR *format, va_list ap) noexcept
{
    va_list ap2;

    va_copy(ap2, ap);
    int n = vsnprintf(0, 0, format, ap2);
    va_end(ap2);

    if (n < 0)
        return newStr(format);

    char *buf = new char[n + 1];
    vsprintf(buf, format, ap);

    return buf;
}
