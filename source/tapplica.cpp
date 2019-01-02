/*-------------------------------------------------------------------*/
/* filename -       tapplica.cpp                                     */
/*                                                                   */
/* function(s)                                                       */
/*          TApplication member functions (constructor & destructor) */
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*                                                                   */
/*    Turbo Vision -  Version 1.0                                    */
/*                                                                   */
/*                                                                   */
/*    Copyright (c) 1987,1988,1990 by Borland International          */
/*    All Rights Reserved.                                           */
/*                                                                   */
/*-------------------------------------------------------------------*/

#define Uses_TSystemError
#define Uses_TEventQueue
#define Uses_TScreen
#define Uses_TObject
#define Uses_TMouse
#define Uses_TApplication
#include <tv.h>

TMouse near TEventQueue::mouse;
static TScreen tsc;
static TEventQueue teq;
static TSystemError sysErr;

void initHistory();
void doneHistory();

TApplication::TApplication() :
    TProgInit( &TApplication::initStatusLine,
                  &TApplication::initMenuBar,
                  &TApplication::initDeskTop
                )
{
    initHistory();
}

TApplication::~TApplication()
{
    doneHistory();
}

void TApplication::suspend()
{
    TSystemError::suspend();
    TEventQueue::suspend();
    TScreen::suspend();
}

void TApplication::resume()
{
    TScreen::resume();
    TEventQueue::resume();
    TSystemError::resume();
}
