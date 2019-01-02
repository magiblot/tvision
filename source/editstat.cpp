/*------------------------------------------------------------*/
/* filename -       editstat.cpp                              */
/*                                                            */
/* defines the static members of class TEditor                */
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

#define Uses_TEditor
#define Uses_TEditorDialog
#include <tv.h>

ushort defEditorDialog( int, ... )
{
    return cmCancel;
}

TEditorDialog near TEditor::editorDialog = defEditorDialog;
ushort near TEditor::editorFlags = efBackupFiles | efPromptOnReplace;
char near TEditor::findStr[maxFindStrLen] = "";
char near TEditor::replaceStr[maxReplaceStrLen] = "";
TEditor * near TEditor::clipboard = 0;

