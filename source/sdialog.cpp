/*------------------------------------------------------------*/
/* filename -       sdialog.cpp                               */
/*                                                            */
/* Registeration object for the class TDialog                 */
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

#define Uses_TDialog
#define Uses_TStreamableClass
#include <tv.h>

__link( RWindow )

TStreamableClass RDialog( TDialog::name,
                          TDialog::build,
                          __DELTA(TDialog)     
                        );

