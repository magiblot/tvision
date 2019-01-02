/*------------------------------------------------------------*/
/* filename -       tevent.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  TEvent member functions                   */
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

#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TScreen
#include <tv.h>

#if !defined( __DOS_H )
#ifndef __DOS_H
#include <dos.h>
#endif
#endif  // __DOS_H

#if !defined( __BIOS_H )
#ifndef __BIOS_H
#include <bios.h>
#endif
#endif  // __BIOS_H


TEvent near TEventQueue::eventQueue[ eventQSize ] = { {0} };
TEvent *near TEventQueue::eventQHead = TEventQueue::eventQueue;
TEvent *near TEventQueue::eventQTail = TEventQueue::eventQueue;
Boolean near TEventQueue::mouseIntFlag = False;

#ifdef PROTECT
ushort far * near TEventQueue::Ticks = (ushort far *)MK_FP( biosSeg, 0x006c );
#else
ushort far * near TEventQueue::Ticks = (ushort far *)MK_FP( 0x0040, 0x006c );
#endif

ushort near TEventQueue::eventCount = 0;
Boolean near TEventQueue::mouseEvents = False;
Boolean near TEventQueue::mouseReverse = False;
ushort near TEventQueue::doubleDelay = 8;
ushort near TEventQueue::repeatDelay = 8;
ushort near TEventQueue::autoTicks = 0;
ushort near TEventQueue::autoDelay = 0;

MouseEventType near TEventQueue::lastMouse;
MouseEventType near TEventQueue::curMouse;
MouseEventType near TEventQueue::downMouse;
ushort near TEventQueue::downTicks = 0;

TEventQueue::TEventQueue()
{
    resume();
}

void TEventQueue::resume()
{
    if( mouse.present() == False )
        mouse.resume();
    if( mouse.present() == False )
        return;
    mouse.getEvent( curMouse );
    lastMouse = curMouse;
    mouse.registerHandler( 0xFFFF, mouseInt );
    mouseEvents = True;
    mouse.show();
    TMouse::setRange( TScreen::screenWidth-1, TScreen::screenHeight-1 );
}

void TEventQueue::suspend()
{
    mouse.suspend();
}

TEventQueue::~TEventQueue()
{
    suspend();
}

void TEventQueue::getMouseEvent( TEvent& ev )
{
    if( mouseEvents == True )
        {

        getMouseState( ev );

        if( ev.mouse.buttons == 0 && lastMouse.buttons != 0 )
            {
            ev.what = evMouseUp;
            lastMouse = ev.mouse;
            return;
            }

        if( ev.mouse.buttons != 0 && lastMouse.buttons == 0 )
            {
            if( ev.mouse.buttons == downMouse.buttons &&
                ev.mouse.where == downMouse.where &&
                ev.what - downTicks <= doubleDelay )
                    ev.mouse.doubleClick = True;

            downMouse = ev.mouse;
            autoTicks = downTicks = ev.what;
            autoDelay = repeatDelay;
            ev.what = evMouseDown;
            lastMouse = ev.mouse;
            return;
            }

        ev.mouse.buttons = lastMouse.buttons;

        if( ev.mouse.where != lastMouse.where )
            {
            ev.what = evMouseMove;
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

void TEventQueue::getMouseState( TEvent & ev )
{
    disable();
    if( eventCount == 0 )
        {
        ev.what = *Ticks;
        ev.mouse = curMouse;
        }
    else
        {
        ev = *eventQHead;
        if( ++eventQHead >= eventQueue + eventQSize )
            eventQHead = eventQueue;
        eventCount--;
        }
    enable();
    if( mouseReverse != False && ev.mouse.buttons != 0 && ev.mouse.buttons != 3 )
        ev.mouse.buttons ^= 3;
}

#pragma saveregs

void huge TEventQueue::mouseInt()
{
    unsigned flag = _AX;
    MouseEventType tempMouse;

    tempMouse.buttons = _BL;
    tempMouse.doubleClick = False;
    tempMouse.where.x = _CX >> 3;
    tempMouse.where.y = _DX >> 3;

    if( (flag & 0x1e) != 0 && eventCount < eventQSize )
        {
        eventQTail->what = *Ticks;
        eventQTail->mouse = curMouse;
        if( ++eventQTail >= eventQueue + eventQSize )
            eventQTail = eventQueue;
        eventCount++;
        }

    curMouse = tempMouse;
    mouseIntFlag = True;
}

#pragma warn -asc

void TEvent::getKeyEvent()
{
    asm {
        MOV AH,1;
        INT 16h;
        JNZ keyWaiting;
        };
    what = evNothing;
    return;

keyWaiting:

    what = evKeyDown;
    asm {
        MOV AH,0;
        INT 16h;
        };
    keyDown.keyCode = _AX;
    return;
}

#pragma warn .asc
