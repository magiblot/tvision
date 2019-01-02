/*------------------------------------------------------------*/
/* filename -       schdrdlg.cpp                              */
/*                                                            */
/* Registeration object for the class TChDirDialog            */
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

#define Uses_TChDirDialog
#define Uses_TStreamableClass
#include <tv.h>
__link( RDialog )
__link( RButton )
__link( RDirListBox )

TStreamableClass RChDirDialog( TChDirDialog::name,
                               TChDirDialog::build,
                               __DELTA(TChDirDialog) 
                             );

