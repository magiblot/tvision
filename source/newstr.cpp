/*------------------------------------------------------------*/
/* filename -       newstr.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  newStr member function                    */
/*------------------------------------------------------------*/
                                                              
/*------------------------------------------------------------*/
/*                                                            */
/*    Turbo Vision -  Version 1.0                             */
/*                                                            */
/*                                                            */
/*    Copyright (c) 1991 by Borland International             */
/*    All Rights Reserved.                                    */
/*                                                            */
/*------------------------------------------------------------*/


#include <tv.h>

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

char *newStr( const char *s )
{
    if( s == 0 )
        return 0;
    char *temp = new char[ strlen(s)+1 ];
    strcpy( temp, s );
    return temp;
}
