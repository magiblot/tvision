#ifndef NCURDISP_H
#define NCURDISP_H

#include <internal/buffdisp.h>
#include <unordered_map>

class NcursesDisplay : public BufferedDisplay {

    int caretSize;
    bool caretVisible;

    bool hasColors;
    std::unordered_map<ushort, int> pairIdentifiers;
    ushort definedPairs;

    void getCaretPosition(int &x, int &y);
    uint translateAttributes(ushort attr);
    uint getColorPair(uchar pairKey);
    static uchar swapRedBlue (uchar c);

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

    void lowlevelWriteChar(uchar character, ushort attr);
    void lowlevelMoveCursor(int x, int y);
    void lowlevelFlush();

};

#endif
