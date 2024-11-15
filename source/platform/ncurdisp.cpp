#ifdef HAVE_NCURSES

#include <internal/ncurdisp.h>
#include <internal/conctl.h>
#include <stdio.h>
#include <stdlib.h>

namespace tvision
{

inline NcursesDisplay::NcursesDisplay(ConsoleCtl &aCon, SCREEN *aTerm) noexcept :
    con(aCon),
    term(aTerm),
    ansiScreenWriter(aCon, TermCap::getDisplayCapabilities(con, *this))
{
}

NcursesDisplay &NcursesDisplay::create(ConsoleCtl &con) noexcept
{
    // Start curses mode.
    SCREEN *term = newterm(nullptr, con.fout(), con.fin());
    if (!term)
    {
        const char *termEnv = getenv("TERM");
        if (!termEnv)
            termEnv = "";
        fprintf(stderr, "Cannot initialize Ncurses: 'newterm' failed (TERM is '%s').\n", termEnv);
        exit(1);
    }
    // Enable colors if the terminal supports it.
    if (has_colors())
    {
        start_color();
        // Use default colors when clearing the screen.
        use_default_colors();
    }
    /* Refresh now so that a possible first getch() doesn't make any relevant
     * changes to the screen due to its implicit refresh(). */
    wrefresh(stdscr);

    return *new NcursesDisplay(con, term);
}

NcursesDisplay::~NcursesDisplay()
{
    // End curses mode.
    endwin();
    delscreen(term);
}

TPoint NcursesDisplay::reloadScreenInfo() noexcept
{
    TPoint size = con.getSize();
    // 'resizeterm' causes terrible flickering, so we better use 'resize_term'.
    resize_term(size.y, size.x);
    ansiScreenWriter.reset();
    return size;
}

int NcursesDisplay::getColorCount() noexcept
{
    return COLORS;
}

TPoint NcursesDisplay::getFontSize() noexcept
{
    return con.getFontSize();
}

void NcursesDisplay::clearScreen() noexcept
{
    ansiScreenWriter.clearScreen();
}

void NcursesDisplay::writeCell( TPoint pos, TStringView text, TColorAttr attr,
                                bool doubleWidth ) noexcept
{
    ansiScreenWriter.writeCell(pos, text, attr, doubleWidth);
}

void NcursesDisplay::setCaretPosition(TPoint pos) noexcept
{
    ansiScreenWriter.setCaretPosition(pos);
}

void NcursesDisplay::setCaretSize(int size) noexcept
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

void NcursesDisplay::flush() noexcept
{
    ansiScreenWriter.flush();
}

} // namespace tvision

#endif // HAVE_NCURSES
