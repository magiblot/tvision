#define Uses_TScreen
#define Uses_THardwareInfo
#include <tvision/tv.h>

#include <ncurses.h>
#include <clocale>
#include <unordered_map>
using std::unordered_map;

THardwareInfo::THardwareInfo()
{
    // Allow printing UTF-8 text.
    setlocale(LC_ALL, "");
    // Start curses mode.
    initscr();
    /* Capture keyboard input, but allow exiting with Ctrl+C while we can't
     * do it from within the application.
     */
    cbreak();
    keypad(stdscr, true);
    // Disable echoing of pressed keys.
    noecho();
}

THardwareInfo::~THardwareInfo() {
    // End curses mode when exiting the application.
    endwin();
}

void THardwareInfo::setCaretPosition( ushort x, ushort y )
{
    // Note that [y, x] = [row, column]
    wmove(stdscr, y, x);
}

ushort THardwareInfo::getScreenMode()
{
/* The original implementation just reads the video mode, and sets the small font
 * if the number of rows is greater than 25.
 * This function is called from TDisplay::getCrtMode, which just returns the
 * value this does, and it is then assigned to a attribute of TScreen.
 */
    // Return whatever seems the most common.
    return TDisplay::smCO80;
}

void THardwareInfo::setCaretSize( ushort size )
{
/* The caret is the keyboard cursor. If size is 0, the caret is hidden. The
 * other possible values are from 1 to 100, theoretically, and represent the
 * percentage of the character cell the caret fills.
 * https://docs.microsoft.com/en-us/windows/console/console-cursor-info-str
 */
    // Stub, do nothing.
}

/* The Turbo Vision library has all its characters encoded in code page 437.
 * Until unicode support is not added, it's better to just translate them
 * with a lookup table. The following table allows translating the characters
 * stored by Turbo Vision into the corresponding UTF-8 mulibyte characters.
 * Taken from: https://en.wikipedia.org/wiki/Code_page_437
 */

const char* cp437toUtf8[256] = {
    "\0", "☺", "☻", "♥", "♦", "♣", "♠", "•", "◘", "○", "◙", "♂", "♀", "♪", "♫", "☼",
    "►", "◄", "↕", "‼", "¶", "§", "▬", "↨", "↑", "↓", "→", "←", "∟", "↔", "▲", "▼",
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
    "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
    "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "⌂",
    "Ç", "ü", "é", "â", "ä", "à", "å", "ç", "ê", "ë", "è", "ï", "î", "ì", "Ä", "Å",
    "É", "æ", "Æ", "ô", "ö", "ò", "û", "ù", "ÿ", "Ö", "Ü", "¢", "£", "¥", "₧", "ƒ",
    "á", "í", "ó", "ú", "ñ", "Ñ", "ª", "º", "¿", "⌐", "¬", "½", "¼", "¡", "«", "»",
    "░", "▒", "▓", "│", "┤", "╡", "╢", "╖", "╕", "╣", "║", "╗", "╝", "╜", "╛", "┐",
    "└", "┴", "┬", "├", "─", "┼", "╞", "╟", "╚", "╔", "╩", "╦", "╠", "═", "╬", "╧",
    "╨", "╤", "╥", "╙", "╘", "╒", "╓", "╫", "╪", "┘", "┌", "█", "▄", "▌", "▐", "▀",
    "α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε", "∩",
    "≡", "±", "≥", "≤", "⌠", "⌡", "÷", "≈", "°", "∙", "·", "√", "ⁿ", "²", "■", " "
};

/* Turbo Vision stores char/attribute information in a CHAR_INFO struct:
 * https://docs.microsoft.com/en-us/windows/console/char-info-str
 * The lower 16 bits store the char value and the higher 16 bits store the
 * character attributes. Turbo Vision usually only fills the lower 8 bits
 * of each part.
 */

void THardwareInfo::screenWrite( ushort x, ushort y, ushort *buf, DWORD len )
{
    setCaretPosition(x, y);
    // It takes two shorts to store a char and its attributes;
    for (int i = 0; i < 2*len; i += 2) {
        uchar character = buf[i];
        // Print a single character, which might be multi-byte in UTF-8.
        wprintw(stdscr, "%s", cp437toUtf8[character]);
    }
    /* Print to the screen, although this is not strictly necessary because
     * it gets later done automatically when waiting for user input in getch().
     */
    wrefresh(stdscr);
}
