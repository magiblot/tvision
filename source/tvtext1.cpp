/*------------------------------------------------------------*/
/* filename -       tvtext1.cpp                               */
/*                                                            */
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

#define Uses_TScreen
#define Uses_TRadioButtons
#define Uses_TMenuBox
#define Uses_TFrame
#define Uses_TIndicator
#define Uses_THistory
#define Uses_TColorSelector
#define Uses_TMonoSelector
#define Uses_TColorDialog
#define Uses_TInputLine
#define Uses_TStatusLine
#define Uses_TCheckBoxes
#define Uses_TScrollBar
#define Uses_TButton
#define Uses_TDirListBox
#define Uses_TFileEditor
#include <tv.h>

#include <dos.h>

static unsigned getCodePage()
{
    //  get version number, in the form of a normal number
    unsigned ver = (_version >> 8) | (_version << 8);
    if( ver < 0x30C )
        return 437; // United States code page, for all versions before 3.3

    _AX = 0x6601;   // get code page
    geninterrupt( 0x21 );
    return _BX;
}

void TDisplay::updateIntlChars()
{
    if(getCodePage() != 437 )
        TFrame::frameChars[30] = 'Í';
}

extern const uchar specialChars[] =
{
    175, 174, 26, 27, ' ', ' '
};

const char * near TRadioButtons::button = " ( ) ";

const char * near TMenuBox::frameChars = " \332\304\277  \300\304\331  \263 \263  \303\304\264 ";

const char near TFrame::initFrame[19] =
  "\x06\x0A\x0C\x05\x00\x05\x03\x0A\x09\x16\x1A\x1C\x15\x00\x15\x13\x1A\x19";

char near TFrame::frameChars[33] = 
    "   À ³ÚÃ ÙÄÁ¿´ÂÅ   È ºÉÇ ¼ÍÏ»¶Ñ "; // for UnitedStates code page

const char * near TFrame::closeIcon = "[~\xFE~]";
const char * near TFrame::zoomIcon = "[~\x18~]";
const char * near TFrame::unZoomIcon = "[~\x12~]";
const char * near TFrame::dragIcon = "~ÄÙ~";

const char near TIndicator::dragFrame = '\xCD';
const char near TIndicator::normalFrame = '\xC4';

const char * near THistory::icon = "\xDE~\x19~\xDD";

const char near TColorSelector::icon = '\xDB';

const char * near TMonoSelector::button = " ( ) ";
const char * near TMonoSelector::normal = "Normal";
const char * near TMonoSelector::highlight = "Highlight";
const char * near TMonoSelector::underline = "Underline";
const char * near TMonoSelector::inverse = "Inverse";

const char * near TColorDialog::colors = "Colors";
const char * near TColorDialog::groupText = "~G~roup";
const char * near TColorDialog::itemText = "~I~tem";
const char * near TColorDialog::forText = "~F~oreground";
const char * near TColorDialog::bakText = "~B~ackground";
const char * near TColorDialog::textText = "Text ";
const char * near TColorDialog::colorText = "Color";
const char * near TColorDialog::okText = "O~K~";
const char * near TColorDialog::cancelText = "Cancel";

const char near TInputLine::rightArrow = '\x10';
const char near TInputLine::leftArrow = '\x11';

const char * near TStatusLine::hintSeparator = "\xB3 ";

const char * near TCheckBoxes::button = " [ ] ";

TScrollChars near TScrollBar::vChars = {30, 31, 177, 254, 178};
TScrollChars near TScrollBar::hChars = {17, 16, 177, 254, 178};

const char * near TButton::shadows = "\xDC\xDB\xDF";
const char * near TButton::markers = "[]";

const char * near TDirListBox::pathDir   = "ÀÄÂ";
const char * near TDirListBox::firstDir  =   "ÀÂÄ";
const char * near TDirListBox::middleDir =   " ÃÄ";
const char * near TDirListBox::lastDir   =   " ÀÄ";
const char * near TDirListBox::drives = "Drives";
const char * near TDirListBox::graphics = "ÀÃÄ";

const char * near TFileEditor::backupExt = ".BAK";

