/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   HARDWARE.H                                                            */
/*                                                                         */
/*   defines the class THardwareInfo                                       */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#pragma option -Vo-
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po-
#endif

#if defined( Uses_THardwareInfo ) && !defined( __THardwareInfo )
#define __THardwareInfo

#if defined( __FLAT__ )

#if !defined( __WINDOWS_H )
#include <tvision/compat/win.h>
#endif

#else

#if !defined( MAKELONG )
#define MAKELONG(h,l) \
    ((long)(((unsigned)(l)) | (((long)((unsigned)(h))) << 16)))
#endif

#endif

#ifndef __BORLANDC__
class PlatformStrategy;
#endif

struct TEvent;
struct MouseEventType;

class THardwareInfo
{

public:

    THardwareInfo();
#ifndef __BORLANDC__
    enum { eventQSize = 24 };
    static TEvent eventQ[eventQSize];
    static size_t eventCount;
    static void flushScreen();
#endif

    static ulong getTickCount();

#if defined( __FLAT__ )

    enum ConsoleType { cnInput = 0, cnOutput = 1, cnStartup = 2 };
    enum PlatformType { plDPMI32 = 1, plWinNT = 2, plOS2 = 4 };

    static PlatformType getPlatform();

// Caret functions.

    static void setCaretSize( ushort size );
    static ushort getCaretSize();
    static void setCaretPosition( ushort x, ushort y );
    static BOOL isCaretVisible();

// Screen functions.

    static ushort getScreenRows();
    static ushort getScreenCols();
    static ushort getScreenMode();
    static void setScreenMode( ushort mode );
    static void clearScreen( ushort w, ushort h );
    static void screenWrite( ushort x, ushort y, TScreenCell *buf, DWORD len );
    static TScreenCell *allocateScreenBuffer();
    static void freeScreenBuffer( TScreenCell *buffer );
    static void resizeScreenBuffer( TScreenCell *&buffer );
    static void reloadScreenInfo();
    static void setUpConsole();
    static void restoreConsole();

// Mouse functions.

    static DWORD getButtonCount();
    static void cursorOn();
    static void cursorOff();

// Event functions.

    static BOOL getMouseEvent( MouseEventType& event );
    static BOOL getKeyEvent( TEvent& event, Boolean blocking=True );
    static void clearPendingEvent();
#ifndef __BORLANDC__
    static BOOL getPendingEvent( TEvent &event, ushort mask );
    static void readEvents( Boolean blocking );
#endif

// System functions.

    static BOOL setCtrlBrkHandler( BOOL install );
    static BOOL setCritErrorHandler( BOOL install );

    static const ushort NormalCvt[89];
    static const ushort ShiftCvt[89];
    static const ushort CtrlCvt[89];
    static const ushort AltCvt[89];

private:

    static BOOL __stdcall ctrlBreakHandler( DWORD dwCtrlType );

    static BOOL insertState;
    static PlatformType platform;
    static HANDLE consoleHandle[3];
    static DWORD consoleMode;
    static DWORD pendingEvent;
    static INPUT_RECORD irBuffer;
    static CONSOLE_CURSOR_INFO crInfo;
    static CONSOLE_SCREEN_BUFFER_INFO sbInfo;
    static int eventTimeoutMs;

#else

    static ushort *getColorAddr( ushort offset = 0 );
    static ushort *getMonoAddr( ushort offset = 0 );
    static uchar getShiftState();
    static uchar getBiosScreenRows();
    static uchar getBiosVideoInfo();
    static void setBiosVideoInfo( uchar info );
    static ushort getBiosEquipmentFlag();
    static ushort huge getBiosEquipmentFlag(int);   // Non-inline version.
    static void setBiosEquipmentFlag( ushort flag );
    static Boolean getDPMIFlag();

private:

    static ushort huge getBiosSelector();   // For SYSINT.ASM.

    static Boolean dpmiFlag;
    static ushort colorSel;
    static ushort monoSel;
    static ushort biosSel;

#endif

};

#if defined( __FLAT__ )

inline THardwareInfo::PlatformType THardwareInfo::getPlatform()
{
    return platform;
}

#ifdef __BORLANDC__
// Caret functions.

inline ushort THardwareInfo::getCaretSize()
{
    return crInfo.dwSize;
}

inline BOOL THardwareInfo::isCaretVisible()
{
    return crInfo.bVisible;
}


// Screen functions.

inline ushort THardwareInfo::getScreenRows()
{
    return sbInfo.dwSize.Y;
}

inline ushort THardwareInfo::getScreenCols()
{
    return sbInfo.dwSize.X;
}

#pragma option -w-inl
inline void THardwareInfo::clearScreen( ushort w, ushort h )
{
    COORD coord = { 0, 0 };
    DWORD read;

    FillConsoleOutputAttribute( consoleHandle[cnOutput], 0x07, w*h, coord, &read );
    FillConsoleOutputCharacterA( consoleHandle[cnOutput], ' ', w*h, coord, &read );
}
#pragma option -w+inl
#endif // __BORLANDC__

inline TScreenCell *THardwareInfo::allocateScreenBuffer()
{
    short x = getScreenCols(), y = getScreenRows();

    if( x < 80 )        // Make sure we allocate at least enough for
        x = 80;         //   a 80x50 screen.
    if( y < 50 )
        y = 50;
#ifdef __BORLANDC__
    return (TScreenCell *) VirtualAlloc( 0, x * y * 4, MEM_COMMIT, PAGE_READWRITE );
#else
    return new TScreenCell[x * y];
#endif
}

inline void THardwareInfo::freeScreenBuffer( TScreenCell *buffer )
{
#ifdef __BORLANDC__
    VirtualFree( buffer, 0, MEM_RELEASE );
#else
    delete[] buffer;
#endif
}

inline void THardwareInfo::resizeScreenBuffer( TScreenCell *&buffer )
{
    freeScreenBuffer(buffer);
    buffer = allocateScreenBuffer();
}

// Mouse functions.

#ifdef __BORLANDC__
inline DWORD THardwareInfo::getButtonCount()
{
    DWORD num;
    GetNumberOfConsoleMouseButtons(&num);
    return num;
}

inline void THardwareInfo::cursorOn()
{
    SetConsoleMode( consoleHandle[cnInput], consoleMode | ENABLE_MOUSE_INPUT );
}

inline void THardwareInfo::cursorOff()
{
    SetConsoleMode( consoleHandle[cnInput], consoleMode & ~ENABLE_MOUSE_INPUT );
}
#endif


// Event functions.

inline void THardwareInfo::clearPendingEvent()
{
    pendingEvent = 0;
}


// System functions.

inline BOOL THardwareInfo::setCtrlBrkHandler( BOOL install )
{
#ifdef _WIN32
    return SetConsoleCtrlHandler( &THardwareInfo::ctrlBreakHandler, install );
#else
/* Sets THardwareInfo::ctrlBreakHandle as the handler of control signals
 * CTRL_C_EVENT and CTRL_BREAK_EVENT. When the signal is received, the
 * handler sets the attribute TSystemError::ctrlBreakHit to true.
 * https://docs.microsoft.com/en-us/windows/console/handlerroutine
 */
    // Stub
    return TRUE;
#endif
}

inline BOOL THardwareInfo::setCritErrorHandler( BOOL install )
{
    return TRUE;        // Handled by NT or DPMI32..
}


#else

inline ushort *THardwareInfo::getColorAddr( ushort offset )
    { return (ushort *) MAKELONG( colorSel, offset ); }

inline ushort *THardwareInfo::getMonoAddr( ushort offset )
    { return (ushort *) MAKELONG( monoSel, offset ); }

inline ulong THardwareInfo::getTickCount()
    { return *(ulong *) MAKELONG( biosSel, 0x6C ); }

inline uchar THardwareInfo::getShiftState()
    { return *(uchar *) MAKELONG( biosSel, 0x17 ); }


inline uchar THardwareInfo::getBiosScreenRows()
    { return *(uchar *) MAKELONG( biosSel, 0x84 ); }

inline uchar THardwareInfo::getBiosVideoInfo()
    { return *(uchar *) MAKELONG( biosSel, 0x87 ); }

inline void THardwareInfo::setBiosVideoInfo( uchar info )
    { *(uchar *) MAKELONG( biosSel, 0x87 ) = info; }

inline ushort THardwareInfo::getBiosEquipmentFlag()
    { return *(ushort *) MAKELONG( biosSel, 0x10 ); }

inline void THardwareInfo::setBiosEquipmentFlag( ushort flag )
    { *(ushort *) MAKELONG( biosSel, 0x10 ) = flag; }

inline Boolean THardwareInfo::getDPMIFlag()
    { return dpmiFlag; }

#endif

#endif  // __THardwareInfo

#pragma option -Vo.
#if defined( __BCOPT__ ) && !defined (__FLAT__)
#pragma option -po.
#endif
