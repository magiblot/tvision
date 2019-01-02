/*------------------------------------------------------------*/
/* filename -       sbutton.cpp                               */
/*                                                            */
/* Registeration object for the class TButton                 */
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

#define Uses_TButton
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )

TStreamableClass RButton( TButton::name,
                          TButton::build,
                          __DELTA(TButton)
                        );

