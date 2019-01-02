/*-------------------------------------------------------------------*/
/* filename -       tapplica.cpp                                     */
/*                                                                   */
/* function(s)                                                       */
/*          TApplication member functions (constructor & destructor) */
/*-------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TSystemError
#define Uses_TEventQueue
#define Uses_THardwareInfo
#define Uses_TScreen
#define Uses_TObject
#define Uses_TMouse
#define Uses_TApplication
#include <tvision\tv.h>

static THardwareInfo hwInfoManager;
TMouse _NEAR TEventQueue::mouse;
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
