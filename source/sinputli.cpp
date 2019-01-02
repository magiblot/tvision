/*------------------------------------------------------------*/
/* filename -       sinputli.cpp                              */
/*                                                            */
/* Registeration object for the class TInputLine              */
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

#define Uses_TInputLine
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RInputLine( TInputLine::name,
                             TInputLine::build,
                             __DELTA(TInputLine)
                           );

