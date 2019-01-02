/*------------------------------------------------------------*/
/* filename -       nmeditor.cpp                              */
/*                                                            */
/* defines the streamable names for classes                   */
/*   TIndicator, TEditor, TMemo, TFileEditor, TEditWindow     */
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

#define Uses_TIndicator
#define Uses_TEditor
#define Uses_TMemo
#define Uses_TFileEditor
#define Uses_TEditWindow
#include <tv.h>

const char * const near TIndicator::name = "TIndicator";
const char * const near TEditor::name = "TEditor";
const char * const near TMemo::name = "TMemo";
const char * const near TFileEditor::name = "TFileEditor";
const char * const near TEditWindow::name = "TEditWindow";


