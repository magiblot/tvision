#ifndef WIN32CON_H
#define WIN32CON_H

#include <internal/termdisp.h>

#ifdef _WIN32

#include <tvision/tv.h>
#include <tvision/compat/win.h>

class Win32Input;
class Win32Display;

class Win32ConsoleStrategy : public PlatformStrategy
{

    UINT cpInput, cpOutput;

    Win32ConsoleStrategy( UINT cpInput, UINT cpOutput,
                          DisplayStrategy *display,
                          InputStrategy *input );
    ~Win32ConsoleStrategy();

    static bool initConsole( UINT &cpInput, UINT &cpOutput,
                             std::unique_ptr<DisplayStrategy> &display,
                             std::unique_ptr<InputStrategy> &input );
    void restoreConsole();
    void resetConsole();

public:

    static Win32ConsoleStrategy *create();

    bool waitForEvent(long ms, TEvent &ev) override;
    int charWidth(TStringView mbc, char32_t wc) override; // ttext.cpp
};

class Win32Input : public InputStrategy
{

    bool insertState;
    ushort surrogate;

    bool getKeyEvent(KEY_EVENT_RECORD, TEvent &ev);
    bool getUnicodeEvent(KEY_EVENT_RECORD, TEvent &ev);
    bool getMouseEvent(MOUSE_EVENT_RECORD, TEvent &ev);

public:

    Win32Input();

    int getButtonCount() override;
    void cursorOn() override;
    void cursorOff() override;
    bool getEvent(TEvent &ev);

};

class Win32Display : public TerminalDisplay
{

    COORD dwSize;
    uchar lastAttr;
    std::vector<char> buf;

public:

    Win32Display();

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

#endif // _WIN32

#endif // WIN32CON_H
