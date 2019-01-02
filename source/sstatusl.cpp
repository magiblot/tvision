/*------------------------------------------------------------*/
/* filename -       sstatusl.cpp                              */
/*                                                            */
/* Registeration object for the class TStatusLine             */
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

#define Uses_TStatusLine
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RStatusLine( TStatusLine::name,
                              TStatusLine::build,
                              __DELTA(TStatusLine) 
                            );

