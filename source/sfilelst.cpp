/*------------------------------------------------------------*/
/* filename -       sfilelst.cpp                              */
/*                                                            */
/* Registeration object for the class TFileList               */
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

#define Uses_TFileList
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RFileList( TFileList::name,
                            TFileList::build,
                            __DELTA(TFileList)             
                          );

