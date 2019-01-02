/*------------------------------------------------------------*/
/* filename -       sradiobu.cpp                              */
/*                                                            */
/* Registeration object for the class TRadioButtons           */
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
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RRadioButtons( TRadioButtons::name,
                                TRadioButtons::build,
                                __DELTA(TRadioButtons)
                              );

