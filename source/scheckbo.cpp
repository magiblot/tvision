/*------------------------------------------------------------*/
/* filename -       scheckbo.cpp                              */
/*                                                            */
/* Registeration object for the class TCheckBoxes             */
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

#define Uses_TCheckBoxes
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RCheckBoxes( TCheckBoxes::name,
                              TCheckBoxes::build,
                              __DELTA(TCheckBoxes)
                            );

