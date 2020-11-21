/*
 *      C/C++ Run Time Library - Version 6.0
 *
 *      Copyright (c) 1987, 1993 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef TVISION_COMPAT_WINDOWS_H
#define TVISION_COMPAT_WINDOWS_H

#if defined(__BORLANDC__) || defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else

// winnt.h
extern "C" {

    typedef void *PVOID;
    typedef PVOID HANDLE;
    typedef char CHAR;
    typedef short SHORT;
    typedef long LONG;
    typedef wchar_t WCHAR;

}

// windef.h
extern "C" {

    typedef unsigned long       DWORD;
    typedef int                 BOOL;
    typedef unsigned char       BYTE;
    typedef unsigned short      WORD;
    typedef int                 INT;
    typedef unsigned int        UINT;

#define MAX_PATH 260

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

}

// wincon.h
extern "C" {

    typedef struct _COORD {
        SHORT X;
        SHORT Y;
    } COORD;

    typedef struct _SMALL_RECT {
        SHORT Left;
        SHORT Top;
        SHORT Right;
        SHORT Bottom;
    } SMALL_RECT;

    typedef struct _KEY_EVENT_RECORD {
        BOOL bKeyDown;
        WORD wRepeatCount;
        WORD wVirtualKeyCode;
        WORD wVirtualScanCode;
        union {
            WCHAR UnicodeChar;
            CHAR   AsciiChar;
        } uChar;
        DWORD dwControlKeyState;
    } KEY_EVENT_RECORD;

//
// ControlKeyState flags
//

#define RIGHT_ALT_PRESSED     0x0001 // the right alt key is pressed.
#define LEFT_ALT_PRESSED      0x0002 // the left alt key is pressed.
#define RIGHT_CTRL_PRESSED    0x0004 // the right ctrl key is pressed.
#define LEFT_CTRL_PRESSED     0x0008 // the left ctrl key is pressed.
#define SHIFT_PRESSED         0x0010 // the shift key is pressed.
#define NUMLOCK_ON            0x0020 // the numlock light is on.
#define SCROLLLOCK_ON         0x0040 // the scrolllock light is on.
#define CAPSLOCK_ON           0x0080 // the capslock light is on.
#define ENHANCED_KEY          0x0100 // the key is enhanced.

    typedef struct _MOUSE_EVENT_RECORD {
        COORD dwMousePosition;
        DWORD dwButtonState;
        DWORD dwControlKeyState;
        DWORD dwEventFlags;
    } MOUSE_EVENT_RECORD;

#define MOUSE_MOVED   0x0001
#define DOUBLE_CLICK  0x0002

    typedef struct _WINDOW_BUFFER_SIZE_RECORD {
        COORD dwSize;
    } WINDOW_BUFFER_SIZE_RECORD;

    typedef struct _MENU_EVENT_RECORD {
        UINT dwCommandId;
    } MENU_EVENT_RECORD;

    typedef struct _FOCUS_EVENT_RECORD {
        BOOL bSetFocus;
    } FOCUS_EVENT_RECORD;

    typedef struct _INPUT_RECORD {
        WORD EventType;
        union {
            KEY_EVENT_RECORD KeyEvent;
            MOUSE_EVENT_RECORD MouseEvent;
            WINDOW_BUFFER_SIZE_RECORD WindowBufferSizeEvent;
            MENU_EVENT_RECORD MenuEvent;
            FOCUS_EVENT_RECORD FocusEvent;
        } Event;
    } INPUT_RECORD;

//
//  EventType flags:
//

#define MOUSE_EVENT       0x0002 // Event contains mouse event record

    typedef struct _CHAR_INFO {
        union {
            WCHAR UnicodeChar;
            CHAR   AsciiChar;
        } Char;
        WORD Attributes;
    } CHAR_INFO;

//
// Attributes flags:
//

#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.

    typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
        COORD dwSize;
        COORD dwCursorPosition;
        WORD  wAttributes;
        SMALL_RECT srWindow;
        COORD dwMaximumWindowSize;
    } CONSOLE_SCREEN_BUFFER_INFO;

    typedef struct _CONSOLE_CURSOR_INFO {
        DWORD  dwSize;
        BOOL   bVisible;
    } CONSOLE_CURSOR_INFO;

#define CTRL_C_EVENT        0
#define CTRL_BREAK_EVENT    1

}

// winbase.h

extern "C" {

    DWORD GetTickCount(void);

}

#endif // __BORLANDC__ || _WIN32

// Flags that are often missing from outdated headers.

#ifndef MOUSE_WHEELED
#define MOUSE_WHEELED   0x0004
#endif
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED  0x0008
#endif
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

#endif // TVISION_COMPAT_WINDOWS_H
