/*-------------------------------------------------------------------*/
/*                                                                   */
/*   Turbo Vision Demo                                               */
/*                                                                   */
/*   Gadgets.cpp:  Gadgets for the Turbo Vision Demo.  Includes a    */
/*        heap view and a clock view which display the clock at the  */
/*        right end of the menu bar and the current heap space at    */
/*        the right end of the status line.                          */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TRect
#define Uses_TView
#define Uses_TDrawBuffer
#include <tvision/tv.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <strstrea.h>
#include <iomanip.h>
#include <time.h>

#include "gadgets.h"

//extern "C" unsigned long farcoreleft( void );

//
// ------------- Heap Viewer functions
//

THeapView::THeapView(TRect& r) : TView( r )
{
    oldMem = 0;
    newMem = heapSize();
}


void THeapView::draw()
{
    TDrawBuffer buf;
    char c = getColor(2);

    buf.moveChar(0, ' ', c, (short)size.x);
    buf.moveStr(0, heapStr, c);
    writeLine(0, 0, (short)size.x, 1, buf);
}


void THeapView::update()
{
    if( (newMem = heapSize()) != oldMem )
        {
        oldMem = newMem;
        drawView();
        }
}


long THeapView::heapSize()
{
    ostrstream totalStr( heapStr, sizeof heapStr);

#ifdef __BORLANDC__
//#if !defined( __DPMI32__ )
    long total = farcoreleft();
//#else
//    long total = 0;
//#endif

#if !defined( __DPMI16__ ) && !defined( __DPMI32__ )
    struct farheapinfo heap;
#endif

//#if defined( __DPMI32__ )
//    switch( _HEAPEMPTY )
//#else
    switch( heapcheck() )
//#endif
        {
        case _HEAPEMPTY:
            strcpy(heapStr, "     No heap");
            total = -1;
            break;

        case _HEAPCORRUPT:
            strcpy(heapStr, "Heap corrupt");
            total = -2;
            break;

        case _HEAPOK:
#if !defined( __DPMI16__ ) && !defined( __DPMI32__ )
            heap.ptr = NULL;
            while(farheapwalk(&heap) != _HEAPEND)
                if(!heap.in_use)
                    total += heap.size;
#endif
            totalStr << setw(12) << total << ends;
            break;
        }
    return(total);
#elif defined(__linux__)
    // mallinfo is defined in malloc.h but doesn't exist in Borlandc.
    // It doesn't exactly measure the heap size, but it kinda does the trick.
    int allocatedBytes = mallinfo().uordblks;
    totalStr << setw(12) << allocatedBytes << ends;
    return allocatedBytes;
#else
    totalStr << ends;
    return 0;
#endif
}


//
// -------------- Clock Viewer functions
//

TClockView::TClockView( TRect& r ) : TView( r )
{
    strcpy(lastTime, "        ");
    strcpy(curTime, "        ");
}


void TClockView::draw()
{
    TDrawBuffer buf;
    char c = getColor(2);

    buf.moveChar(0, ' ', c, (short)size.x);
    buf.moveStr(0, curTime, c);
    writeLine(0, 0, (short)size.x, 1, buf);
}


void TClockView::update()
{
    time_t t = time(0);
    char *date = ctime(&t);

    date[19] = '\0';
    strcpy(curTime, &date[11]);        /* Extract time. */

    if( strcmp(lastTime, curTime) )
        {
        drawView();
        strcpy(lastTime, curTime);
        }
}

