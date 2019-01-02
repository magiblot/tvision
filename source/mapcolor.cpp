/*-------------------------------------------------------------------*/
/* filename -   mapcolor.cpp                                         */
/*                                                                   */
/* function(s)                                                       */
/*          mapColor -- maps a color into a pointer into the current */
/*                      palette.                                     */
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*                                                                   */
/*    Turbo Vision -  Version 1.0                                    */
/*                                                                   */
/*                                                                   */
/*    Copyright (c) 1991 by Borland International                    */
/*    All Rights Reserved.                                           */
/*                                                                   */
/*-------------------------------------------------------------------*/

#define Uses_TView
#define Uses_TGroup
#include <tv.h>

uchar TView::mapColor( uchar color )
{
    if( color == 0 )
        return errorAttr;
    TView *cur = this;
    do  {
        TPalette& p = cur->getPalette();
        if( p[0] != 0 )
            {
            if( color > p[0] )
                return errorAttr;
            color = p[color];
            if( color == 0 )
                return errorAttr;
            }
        cur = cur->owner;
        } while( cur != 0 );
    return color;
}
