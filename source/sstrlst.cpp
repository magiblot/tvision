/*------------------------------------------------------------*/
/* filename -       sstrlst.cpp                               */
/*                                                            */
/* Registeration object for the class TStringList             */
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

#define Uses_TStringList
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RStringList( TStringList::name,
                              TStringList::build,
                              __DELTA(TStringList)
                            );


