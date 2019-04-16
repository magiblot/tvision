/*---------------------------------------------------------*/
/*                                                         */
/*   Calendar.cpp:  TCalenderWindow member functions.      */
/*                                                         */
/*---------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TRect
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TDrawBuffer
#define Uses_TStreamableClass
#define Uses_TStreamable
#define Uses_TView
#define Uses_TWindow
#include <tvision\tv.h>
__link( RView )
__link( RWindow )

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <strstrea.h>
#include <iomanip.h>
#include <dos.h>

#include "calendar.h"


static char *monthNames[] = {
    "",
    "January",  "February", "March",    "April",    "May",      "June",
    "July",     "August",   "September","October",  "November", "December"
};


static unsigned char daysInMonth[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


//
// TCalendarView functions
//

const char * const TCalendarView::name = "TCalendarView";


void TCalendarView::write( opstream& os )
{
    TView::write( os );
    os << days << month << year << curDay << curMonth << curYear;
}


void *TCalendarView::read( ipstream& is )
{
    TView::read( is );
    is >> days >> month >> year >> curDay >> curMonth >> curYear;
    return this;
}


TStreamable *TCalendarView::build()
{
    return new TCalendarView( streamableInit );
}


TStreamableClass RCalendarView( TCalendarView::name,
                                TCalendarView::build,
                                __DELTA(TCalendarView)
                              );


TCalendarView::TCalendarView(TRect& r) : TView( r )
{
    struct date d;

    options |= ofSelectable;
    eventMask |= evMouseAuto;

    getdate( &d );
    year = curYear = d.da_year;
    month = curMonth = d.da_mon;
    curDay = d.da_day;

    drawView();
}


int dayOfWeek(int day, int month, int year)
{
    int century, yr, dw;

    if(month < 3)
        {
        month += 10;
        --year;
        }
    else
        month -= 2;

    century = year / 100;
    yr = year % 100;
    dw = (((26 * (int)month - 2) / 10) + (int)day + yr + (yr / 4) + (century / 4) -
                (2 * century)) % 7;

    if(dw < 0)
        dw += 7;

    return((int)dw);
}


void TCalendarView::draw()
{
    char str[23];
    char current = (char)(1 - dayOfWeek(1, month, year));
    char days = (char)( daysInMonth[month] +
                        ((year % 4 == 0 && month == 2) ? 1 : 0) );
    char color, boldColor;
    short  i, j;
    TDrawBuffer buf;

    color = getColor(6);
    boldColor = getColor(7);

    buf.moveChar(0, ' ', color, 22);

    ostrstream( str, sizeof str)
      << setw(9) << monthNames[month] << " " << setw(4) << year
      << " " << (char) 30 << "  " << (char) 31 << " " << ends;

    buf.moveStr(0, str, color);
    writeLine(0, 0, 22, 1, buf);

    buf.moveChar(0, ' ', color, 22);
    buf.moveStr(0, "Su Mo Tu We Th Fr Sa", color);
    writeLine(0, 1, 22, 1, buf);

    for(i = 1; i <= 6; i++)
        {
        buf.moveChar(0, ' ', color, 22);
        for(j = 0; j <= 6; j++)
            {
            if(current < 1 || current > days)
                buf.moveStr((short)(j*3), "   ", color);
            else
                {
                ostrstream( str, sizeof str )
                  << setw(2) << (int) current << ends;
                if(year == curYear && month == curMonth && current == curDay)
                    buf.moveStr((short)(j*3), str, boldColor);
                else
                    buf.moveStr((short)(j*3), str, color);
                }
            current++;
            }
        writeLine(0, (short)(i+1), 22, 1, buf);
        }
}


void TCalendarView::handleEvent(TEvent& event)
{
    TPoint point;

    TView::handleEvent(event);
    if (state && sfSelected)
        {
        if ( (event.what & evMouse) && (evMouseDown || evMouseAuto) )
            {
            point = makeLocal(event.mouse.where);
            if (point.x == 15 && point.y == 0)
                {
                ++month;
                if (month > 12)
                    {
                    ++year;
                    month = 1;
                    }
                drawView();
                }
            else if (point.x == 18 && point.y == 0)
                {
                --month;
                if (month < 1)
                    {
                    --year;
                    month = 12;
                    }
                drawView();
                }
            }
        else if (event.what == evKeyboard)
            {
            if ( (loByte(event.keyDown.keyCode) == '+') ||
              event.keyDown.keyCode == kbDown)
                {
                ++month;
                if (month > 12)
                    {
                    ++year;
                    month = 1;
                    }
                }
            else if ( (loByte(event.keyDown.keyCode) == '-') ||
              event.keyDown.keyCode == kbUp)
                {
                --month;
                if (month < 1)
                    {
                    --year;
                    month = 12;
                    }
                }
            drawView();
            }
        }
}


//
// TCalendarWindow functions
//

const char * const TCalendarWindow::name = "TCalendarWindow";


void TCalendarWindow::write( opstream& os )
{
    TWindow::write( os );
}


void *TCalendarWindow::read( ipstream& is )
{
    TWindow::read( is );
    return this;
}


TStreamable *TCalendarWindow::build()
{
    return new TCalendarWindow( streamableInit );
}


TStreamableClass RCalendarWindow( TCalendarWindow::name,
                                  TCalendarWindow::build,
                                  __DELTA(TCalendarWindow)
                                );


TCalendarWindow::TCalendarWindow() :
    TWindow( TRect(1, 1, 23, 11), "Calendar", wnNoNumber ),
    TWindowInit( &TCalendarWindow::initFrame )
{
    TRect r(getExtent());

    flags &= ~(wfZoom | wfGrow);
    growMode = 0;

    palette = wpCyanWindow;

    r.grow(-1, -1);
    insert( new TCalendarView( r ));
}
