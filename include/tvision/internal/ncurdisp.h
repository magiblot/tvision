#ifndef TVISION_NCURDISP_H
#define TVISION_NCURDISP_H

#include <internal/termdisp.h>

#ifdef HAVE_NCURSES

#include <ncurses.h>

namespace tvision
{

class NcursesDisplay : public TerminalDisplay
{
    SCREEN *term;

    void getCaretPosition(int &x, int &y) noexcept;
    uint translateAttributes(TColorAttr attr) noexcept;
    uint getColorPair(uchar pairKey) noexcept;

public:

    // The lifetime of 'aIo' exceeds that of 'this'.
    NcursesDisplay(StdioCtl &io) noexcept;
    ~NcursesDisplay();

protected:

    void reloadScreenInfo() noexcept override;
    TPoint getScreenSize() noexcept override;
    int getCaretSize() noexcept override;
    int getColorCount() noexcept override;

    void lowlevelCursorSize(int size) noexcept override;

    // These methods need not be implemented. We rely on AnsiDisplay.

    void clearScreen() noexcept override = 0;
    void lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept override = 0;
    void lowlevelMoveCursor(uint x, uint y) noexcept override = 0;
    void lowlevelFlush() noexcept override = 0;
};

} // namespace tvision

#else

namespace tvision
{

class NcursesDisplay : public DisplayStrategy {};

} // namespace tvision

#endif // HAVE_NCURSES

#endif // TVISION_NCURDISP_H
