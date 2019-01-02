/*------------------------------------------------------------*/
/* filename -       sparamte.cpp                              */
/*                                                            */
/* Registeration object for the class TParamText              */
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

#define Uses_TParamText
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )
__link( RStaticText )

TStreamableClass RParamText( TParamText::name,
                              TParamText::build,
                              __DELTA(TParamText)
                            );

