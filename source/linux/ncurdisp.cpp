#ifdef HAVE_NCURSES

#define Uses_TScreen
#include <tvision/tv.h>

#include <internal/ncurdisp.h>
#include <internal/codepage.h>
#include <internal/textattr.h>
#include <ncurses.h>
#include <clocale>
#include <unordered_map>
using std::unordered_map;

NcursesDisplay::NcursesDisplay() :
    caretSize(1),
    caretVisible(true),
    definedPairs(0)
{
    // Allow printing UTF-8 text.
    setlocale(LC_ALL, "");
    // Start curses mode.
    initscr();
    // Enable colors if the terminal supports it.
    hasColors = getScreenMode() & TDisplay::smCO80;
    if (hasColors)
        start_color();
    BufferedDisplay::init();
    /* Refresh now so that a possible first getch() doesn't make any relevant
     * changes to the screen due to its implicit refresh(). */
    wrefresh(stdscr);
}

NcursesDisplay::~NcursesDisplay()
{
    // End curses mode.
    endwin();
}

int NcursesDisplay::getCaretSize() { return caretSize; }
bool NcursesDisplay::isCaretVisible() { return caretVisible; }
void NcursesDisplay::clearScreen() { flushScreen(); wclear(stdscr); lowlevelFlush(); }
int NcursesDisplay::getScreenRows() { return getmaxy(stdscr); }
int NcursesDisplay::getScreenCols() { return getmaxx(stdscr); }
void NcursesDisplay::lowlevelMoveCursor(uint x, uint y) { wmove(stdscr, y, x); }
void NcursesDisplay::lowlevelFlush() { wrefresh(stdscr); }

ushort NcursesDisplay::getScreenMode()
{
/* The original implementation just reads the video mode, and sets the small font
 * if the number of rows is greater than 25.
 * This function is called from TDisplay::getCrtMode, which just returns the
 * value this does, and it is then assigned to a attribute of TScreen.
 *
 * For the time being, we will only use the video mode to tell Turbo Vision
 * to use color or monochrome formatting according to the terminal capabilities.
 * The video mode does not determine the number of rows or columns used. */
    return has_colors() ? TDisplay::smCO80 : TDisplay::smMono;
}

void NcursesDisplay::setCaretSize(int size)
{
/* The caret is the keyboard cursor. If size is 0, the caret is hidden. The
 * other possible values are from 1 to 100, theoretically, and represent the
 * percentage of the character cell the caret fills.
 * https://docs.microsoft.com/en-us/windows/console/console-cursor-info-str
 *
 * ncurses supports only three levels: invisible (0), normal (1) and
 * very visible (2). They don't make a difference in all terminals, but
 * we can try mapping them to the values requested by Turbo Vision.
 *
 * Regarding the 'caretSize' and 'caretVisible' variables, they are here
 * just to replicate the behaviour of THardwareInfo on Windows. */
    caretSize = size ? size : 1;
    caretVisible = size != 0;
    curs_set(size > 0 ? size == 100 ? 2 : 1 : 0);
}

/* Turbo Vision stores char/attribute information in a CHAR_INFO struct:
 * https://docs.microsoft.com/en-us/windows/console/char-info-str
 * The lower 16 bits store the char value and the higher 16 bits store the
 * character attributes. Turbo Vision usually only uses 8 bits for the char
 * value because it doesn't support Unicode. */

/* From the 8 bits used for attributes, the lower half defines the foreground
 * color while the upper half defines the background color. Each color is
 * defined as Intensity-Red-Green-Blue, while ncurses can handle 4-bit colors
 * in the format Bright-Blue-Green-Red.
 *
 * Bright and Intense are the same except for the fact that not all terminals
 * support bright background colors, and when they do, they do so in different
 * ways. What's certain is that terminals with limited color support (such as
 * the linux console) can display bright foreground colors by using the Bold
 * attribute. Terminals supporting at least 16 colors should support both
 * foreground and background bright colors without any special attribute. The
 * number of supported colors is represented by the global variable COLORS,
 * set by ncurses.
 *
 * Some examples here:
 * https://www.linuxjournal.com/content/about-ncurses-colors-0 */

/* The best way to use colors in ncurses is to define <foreground, background>
 * color pairs. The number of color pairs supported by the terminal is
 * represented by the global variable COLOR_PAIRS. Pair number 0 is reserved
 * for the terminal's default color. Other pairs need to be first defined with
 * init_pair and are assigned an identifier number. The easiest is to begin
 * numerating pairs from one as they get defined. This avoids problems in
 * terminals with limited color support. For instance, the example linked above
 * doesn't work on the linux console because it doesn't take this approach. */

void NcursesDisplay::lowlevelWriteChars(const uchar chars[4], TCellAttribs attr)
{
    // Translate and apply text attributes.
    uint curses_attr = translateAttributes(attr);
    wattron(stdscr, curses_attr);
    // Print a single character, which might be multi-byte in UTF-8.
    waddnstr(stdscr, (const char *) chars, 4);
    wattroff(stdscr, curses_attr);
}

uint NcursesDisplay::translateAttributes(uchar attr)
{
    /* To understand the bit masks, please read:
     * https://docs.microsoft.com/en-us/windows/console/char-info-str
     * Support for bright colors is a bit inconsistent between terminals, so
     * we do the following: if it doesn't support 16 colors, then we provide
     * the terminal with 3-bit colors and use Bold to represent a bright
     * foreground. Otherwise, we provide 4-bit colors directly to the terminal. */
    uchar pairKey = attr & (COLORS < 16 ? 0x77 : 0xFF);
    bool fgIntense = attr & (COLORS < 16 ? 0x08 : 0x00);
    return fgIntense*A_BOLD | (hasColors ? getColorPair(pairKey) : 0);
}

uint NcursesDisplay::getColorPair(uchar pairKey)
{
    /* Color pairs are defined as they are used, counting from one, in order
     * not to make any assumptions on the amount of color pairs supported by
     * the terminal. */
    int id = pairIdentifiers[pairKey];
    if (id == 0)
    {
        // Foreground color in the lower half, background in the upper half.
        TCellAttribs c {pairKey};
        swapRedBlue(c);
        init_pair(++definedPairs, c.fgGet(), c.bgGet());
        id = pairIdentifiers[pairKey] = definedPairs;
    }
    return COLOR_PAIR(id);
}

#endif // HAVE_NCURSES
