/*------------------------------------------------------------*/
/* filename -       slistbox.cpp                              */
/*                                                            */
/* Registeration object for the class TListBox                */
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

#define Uses_TListBox
#define Uses_TStreamableClass
#include <tv.h>
__link( RListViewer )

TStreamableClass RListBox( TListBox::name,
                           TListBox::build,
                           __DELTA(TListBox)
                         );

