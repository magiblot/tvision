/*------------------------------------------------------------*/
/* filename -       smenubar.cpp                              */
/*                                                            */
/* Registeration object for the class TMenuBar                */
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

#define Uses_TMenuBar
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RMenuBar( TMenuBar::name,
                           TMenuBar::build,
                           __DELTA(TMenuBar)
                         );

