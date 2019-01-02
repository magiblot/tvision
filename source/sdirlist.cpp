/*------------------------------------------------------------*/
/* filename -       sdirlist.cpp                              */
/*                                                            */
/* Registeration object for the class TDirListBox             */
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

#define Uses_TDirListBox
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RDirListBox( TDirListBox::name,
                              TDirListBox::build,
                              __DELTA(TDirListBox)             
                            );

