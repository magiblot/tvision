/*------------------------------------------------------------*/
/* filename -       tdialog.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TDialog member functions                  */
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

#define Uses_TKeys
#define Uses_TDialog
#define Uses_TEvent
#include <tv.h>

#define cpDialog "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"\
                 "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"

TDialog::TDialog( const TRect& bounds, const char *aTitle ) :
    TWindow( bounds, aTitle, wnNoNumber ),
    TWindowInit( &TDialog::initFrame )
{
     growMode = 0;
     flags = wfMove | wfClose;
}

TPalette& TDialog::getPalette() const
{
    static TPalette palette( cpDialog, sizeof( cpDialog )-1 );
    return palette;
}

void TDialog::handleEvent(TEvent& event)
{
    TWindow::handleEvent(event);
    switch (event.what)
        {
        case evKeyDown:
            switch (event.keyDown.keyCode)
                {
                case kbEsc:
                    event.what = evCommand;
                    event.message.command = cmCancel;
                    event.message.infoPtr = 0;
                    putEvent(event);
                    clearEvent(event);
                    break;
                case kbEnter:
                    event.what = evBroadcast;
                    event.message.command = cmDefault;
                    event.message.infoPtr = 0;
                    putEvent(event);
                    clearEvent(event);
                    break;
                }
            break;

        case evCommand:
            switch( event.message.command )
                {
                case cmOK:
                case cmCancel:
                case cmYes:
                case cmNo:
                    if( (state & sfModal) != 0 )
                        {
                        endModal(event.message.command);
                        clearEvent(event);
                        }
                    break;
                }
            break;
        }
}

Boolean TDialog::valid( ushort command )
{
    if( command == cmCancel )
        return True;
    else
        return TGroup::valid( command );
}

TStreamable *TDialog::build()
{
    return new TDialog( streamableInit );
}

TDialog::TDialog( StreamableInit ) :
    TWindow( streamableInit ),
    TWindowInit( 0 )
{
}
