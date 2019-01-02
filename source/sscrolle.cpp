/*------------------------------------------------------------*/
/* filename -       sscrolle.cpp                              */
/*                                                            */
/* Registeration object for the class TScroller               */
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

#define Uses_TScroller
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RScroller( TScroller::name,
                             TScroller::build,
                             __DELTA(TScroller)  
                          );

