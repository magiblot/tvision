/*------------------------------------------------------------*/
/* filename -       tradiobu.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TRadioButton member functions             */
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

#define Uses_TRadioButtons
#include <tv.h>

void TRadioButtons::draw()
{
    drawBox( button, 7 );
}

Boolean TRadioButtons::mark( int item )
{
    return Boolean( item == value );
}

void TRadioButtons::press( int item )
{
    value = item;
}

void TRadioButtons::movedTo( int item )
{
    value = item;
}

void TRadioButtons::setData( void * rec )
{
    TCluster::setData(rec);
    sel = value;
}

TStreamable *TRadioButtons::build()
{
    return new TRadioButtons( streamableInit );
}

TRadioButtons::TRadioButtons( StreamableInit ) : TCluster( streamableInit )
{
}
