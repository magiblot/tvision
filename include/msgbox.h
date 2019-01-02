/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   MSGBOX.H                                                              */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the functions messageBox(), messageBoxRect(),                 */
/*   inputBox(), and inputBoxRect()                                        */
/*                                                                         */
/* ------------------------------------------------------------------------*/


#if defined( Uses_MsgBox ) && !defined( __MsgBox )
#define __MsgBox

#if !defined( __STDARG_H )
#include <stdarg.h>
#endif  // __STDARG_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class far TRect;

ushort messageBox( const char *msg, ushort aOptions );
ushort messageBox( ushort aOptions, const char *msg, ... );

ushort messageBoxRect( const TRect &r, const char *msg, ushort aOptions );
ushort messageBoxRect( const TRect &r, ushort aOptions, const char *msg, ... );

ushort inputBox( const char *Title, const char *aLabel, char *s, uchar limit );

ushort inputBoxRect( const TRect &bounds, const char *title,
                     const char *aLabel, char *s, uchar limit );

const

//  Message box classes

    mfWarning      = 0x0000,       // Display a Warning box
    mfError        = 0x0001,       // Dispaly a Error box
    mfInformation  = 0x0002,       // Display an Information Box
    mfConfirmation = 0x0003,       // Display a Confirmation Box

// Message box button flags

    mfYesButton    = 0x0100,       // Put a Yes button into the dialog
    mfNoButton     = 0x0200,       // Put a No button into the dialog
    mfOKButton     = 0x0400,       // Put an OK button into the dialog
    mfCancelButton = 0x0800,       // Put a Cancel button into the dialog

    mfYesNoCancel  = mfYesButton | mfNoButton | mfCancelButton,
                                    // Standard Yes, No, Cancel dialog
    mfOKCancel     = mfOKButton | mfCancelButton;
                                    // Standard OK, Cancel dialog

class MsgBoxText
{

public:

    static const char * near yesText;
    static const char * near noText;
    static const char * near okText;
    static const char * near cancelText;
    static const char * near warningText;
    static const char * near errorText;
    static const char * near informationText;
    static const char * near confirmText;
};

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif

#endif  // Uses_MsgBox


