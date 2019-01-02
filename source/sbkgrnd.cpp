/*------------------------------------------------------------*/
/* filename -       sbkgrnd.cpp                               */
/*                                                            */
/* Registeration object for the class TBackground             */
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

#define Uses_TStreamableClass
#define Uses_TBackground
#include <tv.h>
__link( RView )

TStreamableClass RBackground( TBackground::name,
                              TBackground::build,
                              __DELTA(TBackground)
                            );

