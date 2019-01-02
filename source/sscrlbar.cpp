/*------------------------------------------------------------*/
/* filename -       sscrlbar.cpp                              */
/*                                                            */
/* Registeration object for the class TScrollBar              */
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

#define Uses_TScrollBar
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RScrollBar( TScrollBar::name,
                              TScrollBar::build,
                              __DELTA(TScrollBar)
                           );

