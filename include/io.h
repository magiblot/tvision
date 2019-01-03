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

#endif
