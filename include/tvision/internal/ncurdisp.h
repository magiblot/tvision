#ifndef TVISION_NCURDISP_H
#define TVISION_NCURDISP_H

#include <internal/termdisp.h>

#ifdef HAVE_NCURSES

#include <internal/ansiwrit.h>
#include <ncurses.h>

namespace tvision
{

class NcursesDisplay : public TerminalDisplay
{
public:

    // The lifetime of 'con' exceeds that of 'this'.
    NcursesDisplay(ConsoleCtl &con) noexcept;
    ~NcursesDisplay();

private:

    AnsiScreenWriter ansiScreenWriter;
    SCREEN *term;

    void getCaretPosition(int &x, int &y) noexcept;
    uint translateAttributes(TColorAttr attr) noexcept;
    uint getColorPair(uchar pairKey) noexcept;

protected:

    TPoint reloadScreenInfo() noexcept override;
    void clearScreen() noexcept override;

    int getColorCount() noexcept override;

    void lowlevelWriteChars(TStringView chars, TColorAttr attr) noexcept override;
    void lowlevelMoveCursor(uint x, uint y) noexcept override;
    void lowlevelMoveCursorX(uint x, uint y) noexcept override;
    void lowlevelCursorSize(int size) noexcept override;
    void lowlevelFlush() noexcept override;
};

} // namespace tvision

#else

namespace tvision
{

class NcursesDisplay : public DisplayStrategy {};

} // namespace tvision

#endif // HAVE_NCURSES

#endif // TVISION_NCURDISP_H
