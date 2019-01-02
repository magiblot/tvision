/*------------------------------------------------------------*/
/* filename -       sfilcoll.cpp                              */
/*                                                            */
/* Registeration object for the class TFileCollection         */
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

#define Uses_TFileCollection
#define Uses_TStreamableClass
#include <tv.h>

TStreamableClass RFileCollection( TFileCollection::name,
                                  TFileCollection::build,
                                  __DELTA(TFileCollection)
                                );

