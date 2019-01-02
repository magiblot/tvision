/*------------------------------------------------------------*/
/* filename -       shistory.cpp                              */
/*                                                            */
/* Registeration object for the class THistory                */
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

#define Uses_THistory
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )
__link( RInputLine )

TStreamableClass RHistory( THistory::name,
                           THistory::build,
                           __DELTA(THistory)
                         );

