/*------------------------------------------------------------*/
/* filename -       sstatict.cpp                              */
/*                                                            */
/* Registeration object for the class TStaticText             */
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

#define Uses_TStaticText
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RStaticText( TStaticText::name,
                              TStaticText::build,
                              __DELTA(TStaticText)
                            );

