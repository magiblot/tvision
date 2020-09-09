#ifdef __BORLANDC__
#include <io.h>
#else

#ifndef IO_H
#define IO_H

struct  ftime   {
  unsigned    ft_tsec  : 5;   /* Two second interval */
  unsigned    ft_min   : 6;   /* Minutes */
  unsigned    ft_hour  : 5;   /* Hours */
  unsigned    ft_day   : 5;   /* Days */
  unsigned    ft_month : 4;   /* Months */
  unsigned    ft_year  : 7;   /* Year */
};

#ifdef _MSC_VER
#include <corecrt_io.h>
#else

#include <unistd.h>
#include <sys/stat.h>

inline off_t filelength( int fd )
{
    struct stat s;
    if ( fstat( fd, &s ) == (off_t) -1 )
        return -1;
    return s.st_size;
}

#endif // _MSC_VER

#endif // IO_H

#endif // __BORLANDC__
