#ifndef TVISION_NCURDISP_H
#define TVISION_NCURDISP_H

#include <internal/termdisp.h>

#ifdef HAVE_NCURSES

#include <internal/ansiwrit.h>
#include <ncurses.h>

namespace tvision
{

class NcursesDisplay final : public TerminalDisplay
{
public:

    // The lifetime of 'con' exceeds that of 'this'.
    NcursesDisplay(ConsoleCtl &con) noexcept;
    ~NcursesDisplay();

private:

    AnsiScreenWriter ansiScreenWriter;
    SCREEN *term;

protected:

    TPoint reloadScreenInfo() noexcept override;

    int getColorCount() noexcept override;
    TPoint getFontSize() noexcept override;

    void writeCell(TPoint, TStringView, TColorAttr, bool) noexcept override;
    void setCaretPosition(TPoint) noexcept override;
    void setCaretSize(int) noexcept override;
    void clearScreen() noexcept override;
    void flush() noexcept override;
};

} // namespace tvision

#else

namespace tvision
{

class NcursesDisplay : public DisplayAdapter {};

} // namespace tvision

#endif // HAVE_NCURSES

#endif // TVISION_NCURDISP_H
