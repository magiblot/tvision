#ifdef HAVE_NCURSES

#define Uses_TColorAttr
#include <tvision/tv.h>

#include <internal/ncurdisp.h>
#include <internal/stdioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

namespace tvision
{

NcursesDisplay::NcursesDisplay(StdioCtl &aIo) noexcept :
    TerminalDisplay(aIo),
    ansiScreenWriter(aIo)
{
    // Start curses mode.
    term = newterm(nullptr, io.fout(), io.fin());
    if (!term)
    {
        fputs("Cannot initialize Ncurses: 'newterm' failed.\n", stderr);
        exit(1);
    }
    // Enable colors if the terminal supports it.
    if (has_colors())
    {
        start_color();
        // Use default colors when clearing the screen.
        use_default_colors();
    }
    initCapabilities();
    /* Refresh now so that a possible first getch() doesn't make any relevant
     * changes to the screen due to its implicit refresh(). */
    wrefresh(stdscr);
}

NcursesDisplay::~NcursesDisplay()
{
    // End curses mode.
    endwin();
    delscreen(term);
}

void NcursesDisplay::reloadScreenInfo() noexcept
{
    TPoint size = io.getSize();
    // 'resizeterm' causes terrible flickering, so we better use 'resize_term'.
    resize_term(size.y, size.x);
    ansiScreenWriter.resetAttributes();
}

TPoint NcursesDisplay::getScreenSize() noexcept
{
    int y, x;
    getmaxyx(stdscr, y, x);
    return {max(x, 0), max(y, 0)};
}

int NcursesDisplay::getCaretSize() noexcept
{
    int size = curs_set(0);
    curs_set(size);
    return size <= 0 ? 0 : size == 1 ? 1 : 100;
}

int NcursesDisplay::getColorCount() noexcept
{
    return COLORS;
}

void NcursesDisplay::clearScreen() noexcept
{
    ansiScreenWriter.clearScreen();
}

void NcursesDisplay::lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept
{
    ansiScreenWriter.lowlevelWriteChars(chars, attr, termcap);
}

void NcursesDisplay::lowlevelMoveCursor(uint x, uint y) noexcept
{
    ansiScreenWriter.lowlevelMoveCursor(x, y);
}

void NcursesDisplay::lowlevelMoveCursorX(uint x, uint) noexcept
{
    ansiScreenWriter.lowlevelMoveCursorX(x);
}

void NcursesDisplay::lowlevelCursorSize(int size) noexcept
{
/* The caret is the keyboard cursor. If size is 0, the caret is hidden. The
 * other possible values are from 1 to 100, theoretically, and represent the
 * percentage of the character cell the caret fills.
 * https://docs.microsoft.com/en-us/windows/console/console-cursor-info-str
 *
 * ncurses supports only three levels: invisible (0), normal (1) and
 * very visible (2). They don't make a difference in all terminals, but
 * we can try mapping them to the values requested by Turbo Vision. */
    curs_set(size > 0 ? size == 100 ? 2 : 1 : 0); // Implies refresh().
}

void NcursesDisplay::lowlevelFlush() noexcept
{
    ansiScreenWriter.lowlevelFlush();
}

} // namespace tvision

#endif // HAVE_NCURSES
