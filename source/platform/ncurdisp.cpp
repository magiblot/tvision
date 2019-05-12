#define Uses_TScreen
#define Uses_THardwareInfo
#include <tvision/tv.h>

#include <platform.h>
#include <ncurses.h>
#include <clocale>
#include <unordered_map>
using std::unordered_map;

void NcursesDisplay::startDisplay()
{
    // Allow printing UTF-8 text.
    setlocale(LC_ALL, "");
    // Start curses mode.
    initscr();
    // Enable colors if the terminal supports it.
    hasColors = getScreenMode() & TDisplay::smCO80;
    if (hasColors)
        start_color();
}

void NcursesDisplay::endDisplay()
{
    // End curses mode.
    endwin();
}

int NcursesDisplay::getCaretSize()
{
    int visibility = curs_set(0);
    curs_set(visibility);
    return visibility > 0 ? visibility == 2 ? 100 : 1 : 0;
}

bool NcursesDisplay::isCaretVisible() { return getCaretSize() > 0; }
void NcursesDisplay::clearScreen() { wclear(stdscr); }
int NcursesDisplay::getScreenRows() { return getmaxy(stdscr); }
int NcursesDisplay::getScreenCols() { return getmaxx(stdscr); }
void NcursesDisplay::setCaretPosition(int x, int y) { wmove(stdscr, y, x); }
void NcursesDisplay::getCaretPosition(int &x, int &y) { getyx(stdscr, y, x); }
void NcursesDisplay::flushScreen() { wrefresh(stdscr); }

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
 * we can try mapping them to the values requested by Turbo Vision. */
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

// tables.cpp
extern const char* cp437toUtf8[256];

void NcursesDisplay::screenWrite( int x, int y, ushort *buf, int len )
{
    // Save the caret position so that we can restore it later.
    int oldx, oldy;
    getCaretPosition(oldx, oldy);
    setCaretPosition(x, y);
    // It takes two shorts to store a character and its attributes:
    for (int i = 0; i < 2*len; i += 2)
    {
        uchar character = buf[i];
        ushort attr = buf[i + 1];
        // Translate and apply text attributes.
        uint curses_attr = translateAttributes(attr);
        wattron(stdscr, curses_attr);
        // Print a single character, which might be multi-byte in UTF-8.
        wprintw(stdscr, "%s", cp437toUtf8[character]);
        wattroff(stdscr, curses_attr);
    }
    // Move the caret back to where it was.
    setCaretPosition(oldx, oldy);
    THardwareInfo::flushScreen();
}

uint NcursesDisplay::translateAttributes(ushort attr)
{
    /* To understand the bit masks, please read:
     * https://docs.microsoft.com/en-us/windows/console/char-info-str
     * Support for bright colors is a bit inconsistent between terminals, so
     * we do the following: if it doesn't support 16 colors, then we provide
     * the terminal with 3-bit colors and use Bold to represent a bright
     * foreground. Otherwise, we provide 4-bit colors directly to the terminal. */
    uchar pairKey = attr & (COLORS < 16 ? 0x77 : 0xFF);
    bool fgIntense = attr & (COLORS < 16 ? 0x08 : 0x00),
         reverse = attr & 0x4000,
         underscore = attr & 0x8000;
    return fgIntense*A_BOLD | reverse*A_REVERSE | underscore*A_UNDERLINE |
           (hasColors ? getColorPair(pairKey) : 0);
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
        uchar fgColor = swapRedBlue(0x0F & pairKey),
              bgColor = swapRedBlue(pairKey >> 4);
        init_pair(++definedPairs, fgColor, bgColor);
        id = pairIdentifiers[pairKey] = definedPairs;
    }
    return COLOR_PAIR(id);
}

uchar NcursesDisplay::swapRedBlue (uchar c)
{
    /* Swap the Red and Blue bits of a color encoded in the Microsoft way,
     * so that ncurses can easily understand it. */
    return (c & ~0x5) | ((c & 0x4) >> 2) | ((c & 0x1) << 2);
}

