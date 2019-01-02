/*------------------------------------------------------------*/
/* filename -       sdircoll.cpp                              */
/*                                                            */
/* Registeration object for the class TDirCollection          */
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

#define Uses_TDirCollection
#define Uses_TDirEntry
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RDirCollection( TDirCollection::name,
                                 TDirCollection::build,
                                 __DELTA(TDirCollection) 
                               );

