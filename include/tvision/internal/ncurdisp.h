#ifndef NCURDISP_H
#define NCURDISP_H

#include <internal/termdisp.h>

#ifdef HAVE_NCURSES

#include <unordered_map>
#include <ncurses.h>

class NcursesDisplay : public TerminalDisplay {

    SCREEN *term;

    bool hasColors;
    std::unordered_map<ushort, int> pairIdentifiers;
    ushort definedPairs;

    bool usesNcursesDraw;

    void getCaretPosition(int &x, int &y);
    uint translateAttributes(TColorAttr attr);
    uint getColorPair(uchar pairKey);

public:

    NcursesDisplay();
    ~NcursesDisplay();

    void reloadScreenInfo() override;
    TPoint getScreenSize() override;
    int getCaretSize() override;
    int getColorCount() override;

    void clearScreen() override;

protected:

    void lowlevelWriteChars(TStringView chars, TColorAttr attr) override;
    void lowlevelMoveCursor(uint x, uint y) override;
    void lowlevelCursorSize(int size) override;
    void lowlevelFlush() override;

};

#else

class NcursesDisplay : public DisplayStrategy {};

#endif // HAVE_NCURSES

#endif
