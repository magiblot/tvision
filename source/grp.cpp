/*------------------------------------------------------------*/
/* filename - grp.cpp                                         */
/*                                                            */
/* function(s)                                                */
/*            TGroup member functions                         */
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

#define Uses_TGroup
#include <tv.h>

TView *TGroup::at( short index )
{
    TView *temp = last;
    while( index-- > 0 )
        temp = temp->next;
    return temp;
}

TView *TGroup::firstThat( Boolean (*func)(TView *, void *), void *args )
{
    TView *temp = last;
    if( temp == 0 )
        return 0;

    do  {
        temp = temp->next;
        if( func( temp, args ) == True )
            return temp;
        } while( temp != last );
    return 0;
}

void TGroup::forEach( void (*func)(TView*, void *), void *args )
{
    TView *term = last;
    TView *temp = last;
    if( temp == 0 )
        return;

    TView *next = temp->next;
    do  {
        temp = next;
        next = temp->next;
        func( temp, args );
        } while( temp != term );

}

short TGroup::indexOf( TView *p )
{
    if( last == 0 )
        return 0;

    short index = 0;
    TView *temp = last;
    do  {
        index++;
        temp = temp->next;
        } while( temp != p && temp != last );
    if( temp != p )
        return 0;
    else
        return index;
}



