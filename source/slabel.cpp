/*------------------------------------------------------------*/
/* filename -       slabel.cpp                                */
/*                                                            */
/* Registeration object for the class TLabel                  */
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

#define Uses_TLabel
#define Uses_TStreamableClass
#include <tv.h>
__link( RStaticText )

TStreamableClass RLabel( TLabel::name,
                         TLabel::build,
                         __DELTA(TLabel)
                       );

