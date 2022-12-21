/*------------------------------------------------------------*/
/* filename -       tevent.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  TEvent member functions                   */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
#if defined( __DPMI16__ )
#pragma inline
#endif

#define Uses_TKeys
#define Uses_TEvent
#define Uses_TScreen
#define Uses_TEventQueue
#define Uses_THardwareInfo
#define Uses_TText
#include <tvision/tv.h>

#if !defined (__FLAT__)

#if !defined( __BIOS_H )
#include <bios.h>
#endif  // __BIOS_H

#if !defined( __DOS_H )
#include <dos.h>
#endif  // __DOS_H

#endif

#if !defined( __FLAT__ )
TEvent _NEAR TEventQueue::eventQueue[ eventQSize ] = { {0} };
TEvent * _NEAR TEventQueue::eventQHead = TEventQueue::eventQueue;
TEvent * _NEAR TEventQueue::eventQTail = TEventQueue::eventQueue;
Boolean _NEAR TEventQueue::mouseIntFlag = False;
ushort _NEAR TEventQueue::eventCount = 0;
#endif

ushort _NEAR TEventQueue::downTicks = 0;

Boolean _NEAR TEventQueue::mouseEvents = False;
Boolean _NEAR TEventQueue::mouseReverse = False;
Boolean _NEAR TEventQueue::pendingMouseUp = False;
ushort _NEAR TEventQueue::doubleDelay = 8;
ushort _NEAR TEventQueue::repeatDelay = 8;
ushort _NEAR TEventQueue::autoTicks = 0;
ushort _NEAR TEventQueue::autoDelay = 0;

MouseEventType _NEAR TEventQueue::lastMouse;
MouseEventType _NEAR TEventQueue::curMouse;
MouseEventType _NEAR TEventQueue::downMouse;

TMouse *TEventQueue::mouse;

char *TEventQueue::pendingText = 0;
size_t TEventQueue::pendingTextLength = 0;
size_t TEventQueue::pendingTextIndex = 0;

TEventQueue::TEventQueue() noexcept
{
    static TMouse mouse;
    this->mouse = &mouse;
    resume();
}


void TEventQueue::resume() noexcept
{
    if( mouse->present() == False )
        mouse->resume();
    if( mouse->present() == False )
        return;

    mouse->getEvent( curMouse );
    lastMouse = curMouse;

#if defined( __FLAT__ )
    THardwareInfo::clearPendingEvent();
#else
    mouse->registerHandler( 0xFFFF, (void (_FAR *)()) mouseInt );
#endif

    mouseEvents = True;
    TMouse::setRange( TScreen::screenWidth-1, TScreen::screenHeight-1 );
}


void TEventQueue::suspend() noexcept
{
    mouse->suspend();
}

TEventQueue::~TEventQueue()
{
    suspend();
    delete pendingText;
    pendingText = 0;
}


void TEventQueue::getMouseEvent( TEvent & ev) noexcept
{
    if( mouseEvents == True )
        {
        if( pendingMouseUp == True )
            {
            ev.what = evMouseUp;
            ev.mouse = lastMouse;
            lastMouse.buttons = 0;
            pendingMouseUp = False;
            return;
            }
        if( !getMouseState( ev ) )
            return;

        ev.mouse.eventFlags = 0;

        if( ev.mouse.buttons == 0 && lastMouse.buttons != 0 )
            {
            if( ev.mouse.where == lastMouse.where )
                {
                ev.what = evMouseUp;
                uchar buttons = lastMouse.buttons;
                lastMouse = ev.mouse;
                ev.mouse.buttons = buttons;
                }
            else
                {
                ev.what = evMouseMove;
                MouseEventType up = ev.mouse;
                TPoint where = up.where;
                ev.mouse = lastMouse;
                ev.mouse.where = where;
                ev.mouse.eventFlags |= meMouseMoved;
                up.buttons = lastMouse.buttons;
                lastMouse = up;
                pendingMouseUp = True;
                }
            return;
            }

        if( ev.mouse.buttons != 0 && lastMouse.buttons == 0 )
            {
            if( ev.mouse.buttons == downMouse.buttons &&
                ev.mouse.where == downMouse.where &&
                ev.what - downTicks <= doubleDelay
              )
                {
                if( !(downMouse.eventFlags & (meDoubleClick | meTripleClick)) )
                    ev.mouse.eventFlags |= meDoubleClick;
                else if( downMouse.eventFlags & meDoubleClick )
                    {
                    ev.mouse.eventFlags &= ~meDoubleClick;
                    ev.mouse.eventFlags |= meTripleClick;
                    }
                }

            downMouse = ev.mouse;
            autoTicks = downTicks = ev.what;
            autoDelay = repeatDelay;
            ev.what = evMouseDown;
            lastMouse = ev.mouse;
            return;
            }

        ev.mouse.buttons = lastMouse.buttons;

        if( ev.mouse.wheel != 0 )
            {
            ev.what = evMouseWheel;
#ifdef __BORLANDC__
        // A bug in Borland C++ causes mouse position to be trash on
        // MOUSE_WHEELED events.
            ev.mouse.where = lastMouse.where;
#endif
            lastMouse = ev.mouse;
            return;
            }

        if( ev.mouse.where != lastMouse.where )
            {
            ev.what = evMouseMove;
            ev.mouse.eventFlags |= meMouseMoved;
            lastMouse = ev.mouse;
            return;
            }

        if( ev.mouse.buttons != 0 && ev.what - autoTicks > autoDelay )
            {
            autoTicks = ev.what;
            autoDelay = 1;
            ev.what = evMouseAuto;
            lastMouse = ev.mouse;
            return;
            }
        }

    ev.what = evNothing;
}


Boolean TEventQueue::getMouseState( TEvent & ev ) noexcept
{
#if defined( __FLAT__ )
    ev.what = evNothing;

    if( !THardwareInfo::getMouseEvent( curMouse ) )
        return False;

    if( mouseReverse == True && curMouse.buttons != 0 && curMouse.buttons != 3 )
        curMouse.buttons ^= 3;

    ev.what = THardwareInfo::getTickCount();  // Temporarily save tick count when event was read.
    ev.mouse = curMouse;
    return True;
#else
    Boolean result = False;
    disable();

    if( eventCount == 0 )
        {
        ev.what = THardwareInfo::getTickCount();
        ev.mouse = curMouse;
        result = True;
        // 'wheel' represents an event, not a state. So, in order not to process
        // a mouse wheel event more than once, this field must be set back to zero.
        curMouse.wheel = 0;
        }
    else
        {
        ev = *eventQHead;
        if( ++eventQHead >= eventQueue + eventQSize )
            eventQHead = eventQueue;
        eventCount--;
        result = True;
        }
    enable();

    if( mouseReverse && ev.mouse.buttons != 0 && ev.mouse.buttons != 3 )
        ev.mouse.buttons ^= 3;

    return result;
#endif
}


#if !defined( __FLAT__ )
#pragma saveregs
void __MOUSEHUGE TEventQueue::mouseInt()
{
#if defined( __DPMI16__ )
I   PUSH DS          // Cannot use huge anymore, because someone might compile
I   PUSH AX          //  this module with -WX and that generates Smart Callback
I   MOV  AX, DGROUP  //  style prolog code.  This is an asynchronous callback!
I   MOV  DS, AX
I   POP  AX
#endif

    unsigned flag = _AX;
    MouseEventType tempMouse;

    tempMouse.buttons = _BL;
    tempMouse.wheel = _BH == 0 ? 0 : char(_BH) > 0 ? mwDown : mwUp; // CuteMouse
    tempMouse.eventFlags = 0;
    tempMouse.where.x = _CX >> 3;
    tempMouse.where.y = _DX >> 3;
    tempMouse.controlKeyState = THardwareInfo::getShiftState();

    if( (flag & 0x1e) != 0 && eventCount < eventQSize )
        {
        eventQTail->what = THardwareInfo::getTickCount();
        eventQTail->mouse = curMouse;
        if( ++eventQTail >= eventQueue + eventQSize )
            eventQTail = eventQueue;
        eventCount++;
        }

    curMouse = tempMouse;
    mouseIntFlag = True;

#if defined( __DPMI16__ )
I   POP DS
#endif
}
#endif

void TEventQueue::putTextEvent( TStringView text ) noexcept
{
    delete[] pendingText;
    pendingText = newStr(text);
    pendingTextLength = text.size();
    pendingTextIndex = 0;
}

Boolean TEventQueue::getTextEvent( TEvent &event ) noexcept
{
    if( pendingText )
        {
        TSpan<char> text( pendingText + pendingTextIndex,
                          pendingTextLength - pendingTextIndex );
        size_t length = TText::next( text );
        if( length > 0 )
            {
            KeyDownEvent keyDown = { 0x0000, kbBracketed, {0}, (uchar) length };
            event.what = evKeyDown;
            event.keyDown = keyDown;
            memcpy( event.keyDown.text, text.data(), length );
            pendingTextIndex += length;
            return True;
            }
        delete[] pendingText;
        pendingText = 0;
        }
    return False;
}

void TEvent::getKeyEvent() noexcept
{
    if( TEventQueue::getTextEvent( *this ) )
        return;
#if defined( __FLAT__ )
    if( THardwareInfo::getKeyEvent( *this ) )
    {
        if( what == evKeyDown )
        {
            // Need to handle special case of Alt-Space, Ctrl-Ins, Shift-Ins,
            // Ctrl-Del, Shift-Del

            switch( keyDown.keyCode )
            {
            case ' ':
                if( keyDown.controlKeyState & kbAltShift )
                    keyDown.keyCode = kbAltSpace;
                break;

            case kbDel:
                if( keyDown.controlKeyState & kbCtrlShift )
                    keyDown.keyCode = kbCtrlDel;
                else if( keyDown.controlKeyState & kbShift )
                    keyDown.keyCode = kbShiftDel;
                break;

            case kbIns:
                if( keyDown.controlKeyState & kbCtrlShift )
                    keyDown.keyCode = kbCtrlIns;
                else if( keyDown.controlKeyState & kbShift )
                    keyDown.keyCode = kbShiftIns;
                break;
            }
        }
    }
    else
        what = evNothing;
#else

I   MOV AH,1;
I   INT 16h;
I   JNZ keyWaiting;

    what = evNothing;
    return;

keyWaiting:

    what = evKeyDown;

I   MOV AH,0;
I   INT 16h;

    keyDown.keyCode = _AX;
    keyDown.controlKeyState = THardwareInfo::getShiftState();
#endif
#if defined( __BORLANDC__ )
    if( what == evKeyDown )
        {
        if( ' ' <= keyDown.charScan.charCode &&
            keyDown.charScan.charCode != 0x7F &&
            keyDown.charScan.charCode != 0xFF
          )
            {
            keyDown.text[0] = (char) keyDown.charScan.charCode;
            keyDown.textLength = 1;
            }
        else
            keyDown.textLength = 0;
        }
#endif
}

void TEvent::waitEvent(int timeoutMs) noexcept
{
#if defined( __FLAT__ )
    THardwareInfo::waitForEvents(timeoutMs);
#endif
}

void TEvent::putNothing() noexcept
{
#if defined( __FLAT__ )
    THardwareInfo::stopEventWait();
#endif
}

