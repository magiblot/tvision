/*------------------------------------------------------------*/
/* filename -       msgbox.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  messageBox related functions              */
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


#define Uses_MsgBox
#define Uses_TObject
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TRect
#define Uses_TButton
#define Uses_TProgram
#define Uses_TInputLine
#define Uses_TDeskTop
#define Uses_TLabel
#include <tv.h>

#if !defined( __STDARG_H )
#include <stdarg.h>
#endif  // __STDARG_H

#if !defined( __STDIO_H )
#include <stdio.h>
#endif  // __STDIO_H

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

static const char *buttonName[] =
{
    MsgBoxText::yesText,
    MsgBoxText::noText,
    MsgBoxText::okText,
    MsgBoxText::cancelText
};

static ushort commands[] =
{
    cmYes,
    cmNo,
    cmOK,
    cmCancel
};

static const char *Titles[] =
{
    MsgBoxText::warningText,
    MsgBoxText::errorText,
    MsgBoxText::informationText,
    MsgBoxText::confirmText
};

ushort messageBoxRect( const TRect &r, const char *msg, ushort aOptions )
{
    TDialog *dialog;
    short i, x, buttonCount;
    TView* buttonList[5];
    ushort ccode; 

    dialog = new TDialog( r, Titles[aOptions & 0x3] );

    dialog->insert(
        new TStaticText(TRect(3, 2, dialog->size.x - 2, dialog->size.y - 3),
                        msg) );

    for( i = 0, x = -2, buttonCount = 0; i < 4; i++ )
        {
        if( (aOptions & (0x0100 << i)) != 0)
            {
            buttonList[buttonCount] =
                new TButton( TRect(0, 0, 10, 2), buttonName[i], commands[i], bfNormal );
            x += buttonList[buttonCount++]->size.x + 2;
            }
        }

    x = (dialog->size.x - x) / 2;

    for( i = 0; i < buttonCount; i++ )
        {
        dialog->insert(buttonList[i]);
        buttonList[i]->moveTo(x, dialog->size.y - 3);
        x += buttonList[i]->size.x + 2;
        }

    dialog->selectNext(False);

    ccode = TProgram::deskTop->execView(dialog);

    TObject::destroy( dialog );
    
    return ccode;
}

ushort messageBoxRect( const TRect &r,
                       ushort aOptions,
                       const char *fmt,
                       ... )
{
    va_list argptr;
    va_start( argptr, aOptions );

    char msg[256];
    vsprintf( msg, fmt, argptr );

    return messageBoxRect( r, msg, aOptions );
}

static TRect makeRect()
{
    TRect r( 0, 0, 40, 9 );
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return r;
}

ushort messageBox( const char *msg, ushort aOptions )
{
    return messageBoxRect( makeRect(), msg, aOptions );
}

ushort messageBox( ushort aOptions, const char *fmt, ... )
{
    va_list argptr;
    va_start( argptr, aOptions );

    char msg[256];
    vsprintf( msg, fmt, argptr );

    return messageBoxRect( makeRect(), msg, aOptions );
}

ushort inputBox( const char *Title, const char *aLabel, char *s, uchar limit )
{
    TRect r(0, 0, 60, 8);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return inputBoxRect(r, Title, aLabel, s, limit);
}

ushort inputBoxRect( const TRect &bounds,
                     const char *Title,
                     const char *aLabel,
                     char *s,
                     uchar limit )
{
    TDialog *dialog;
    TView* control;
    TRect r;
    ushort c;

    dialog = new TDialog(bounds, Title);

    r = TRect( 4 + strlen(aLabel), 2, dialog->size.x - 3, 3 );
    control = new TInputLine( r, limit );
    dialog->insert( control );

    r = TRect(2, 2, 3 + strlen(aLabel), 3);
    dialog->insert( new TLabel( r, aLabel, control ) );

    r = TRect( dialog->size.x - 24, dialog->size.y - 4,
               dialog->size.x - 14, dialog->size.y - 2);
    dialog->insert( new TButton(r, MsgBoxText::okText, cmOK, bfDefault));

    r.a.x += 12;
    r.b.x += 12;
    dialog->insert( new TButton(r, MsgBoxText::cancelText, cmCancel, bfNormal));

    r.a.x += 12;
    r.b.x += 12;
    dialog->selectNext(False);
    dialog->setData(s);
    c = TProgram::deskTop->execView(dialog);
    if( c != cmCancel )
        dialog->getData(s);
    TObject::destroy( dialog );
    return c;
}

