#ifndef NCURDISP_H
#define NCURDISP_H

#include <internal/platform.h>
#include <unordered_map>

class NcursesDisplay : public DisplayStrategy {

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
    void setCaretPosition(int x, int y);
    bool isCaretVisible();
    void clearScreen();
    int getScreenRows();
    int getScreenCols();
    ushort getScreenMode();
    void screenWrite(int x, int y, ushort *buf, int len);
    void flushScreen();

};

#endif
