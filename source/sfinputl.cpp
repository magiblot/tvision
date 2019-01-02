/*------------------------------------------------------------*/
/* filename -       sfinputl.cpp                              */
/*                                                            */
/* Registeration object for the class TFileInputLine          */
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

#define Uses_TFileInputLine
#define Uses_TStreamableClass
#include <tv.h>
__link( RInputLine )

TFileInputLine::TFileInputLine( StreamableInit ) :
                TInputLine( streamableInit)
{
} 

TStreamable *TFileInputLine::build()
{
    return new TFileInputLine( streamableInit );
}

TStreamableClass RFileInputLine( TFileInputLine::name,
                                 TFileInputLine::build,
                                __DELTA(TFileInputLine)
                               );

