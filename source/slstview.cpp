/*------------------------------------------------------------*/
/* filename -       slstview.cpp                              */
/*                                                            */
/* Registeration object for the class TListViewer             */
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

#define Uses_TListViewer
#define Uses_TStreamableClass
#include <tv.h>
__link( RView )
__link( RScrollBar )

TStreamableClass RListViewer( TListViewer::name,
                              TListViewer::build,
                              __DELTA(TListViewer)
                            );

