/*------------------------------------------------------------*/
/* filename -       sfildlg.cpp                               */
/*                                                            */
/* Registeration object for the class TFileDialog             */
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

#define Uses_TFileDialog
#define Uses_TStreamableClass
#include <tv.h>
__link( RDialog )
__link( RFileInputLine )
__link( RFileList )

TStreamableClass RFileDialog( TFileDialog::name,
                              TFileDialog::build,
                              __DELTA(TFileDialog)
                            );

