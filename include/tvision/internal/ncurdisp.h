#ifndef NCURDISP_H
#define NCURDISP_H

#include <internal/buffdisp.h>

#ifdef HAVE_NCURSES

#include <unordered_map>

class NcursesDisplay : public BufferedDisplay {

    int caretSize;
    bool caretVisible;

    bool hasColors;
    std::unordered_map<ushort, int> pairIdentifiers;
    ushort definedPairs;

    void getCaretPosition(int &x, int &y);
    uint translateAttributes(uchar attr);
    uint getColorPair(uchar pairKey);

public:

    NcursesDisplay();
    ~NcursesDisplay();

    void setCaretSize(int size);
    int getCaretSize();
    bool isCaretVisible();
    void clearScreen();
    int getScreenRows();
    int getScreenCols();
    ushort getScreenMode();

protected:

    void lowlevelWriteChars(const uchar chars[4], TCellAttribs attr);
    void lowlevelMoveCursor(uint x, uint y);
    void lowlevelFlush();

};

#else

class NcursesDisplay : public DisplayStrategy {};

#endif // HAVE_NCURSES

#endif
