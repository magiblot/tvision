#define Uses_TEvent
#define Uses_TKeys
#define Uses_TFrame
#define Uses_TDeskTop
#define Uses_TProgram
#include <tvision/tv.h>
#include <iostream.h>

#include "evntview.h"

TEventViewer* TEventViewer::viewer = 0;

TEventViewer* TEventViewer::toggle()
{
    static const char *titleResumed = "Event Viewer",
                      *titleStopped = "Event Viewer (Stopped)";
    if (!viewer)
    {
        TRect bounds = TProgram::deskTop->getExtent();
        viewer = new TEventViewer(bounds, titleResumed, 0x0F00);
        return viewer;
    }
    else
    {
        viewer->resumed = Boolean(!viewer->resumed);
        viewer->title = viewer->resumed ? titleResumed : titleStopped;
        viewer->frame->drawView();
        return 0;
    }
}

void TEventViewer::print(const TEvent &ev)
{
    if (ev.what != evNothing && viewer && viewer->resumed && viewer->out)
    {
        viewer->lock();
        *viewer->out << "Received event #" << ++viewer->eventCount << '\n';
        printEvent(*viewer->out, ev);
        *viewer->out << flush;
        viewer->unlock();
    }
}

TEventViewer::TEventViewer( TRect bounds, const char *aTitle,
                            ushort aBufSize ) :
    TWindowInit( &TEventViewer::initFrame ),
    TWindow(bounds, aTitle, wnNoNumber ),
    interior(0), out(0), resumed(True), eventCount(0)
{
    bounds = getExtent();
    bounds.grow( -1, -1 );

    interior = new TTerminal( bounds, 0,
                              standardScrollBar( sbVertical | sbHandleKeyboard ),
                              aBufSize );
    out = new otstream(interior);
    insert( interior );
}

TEventViewer::~TEventViewer()
{
    delete out;
    title = 0; // Not heap-allocated.
    viewer = 0;
    out = 0;
}

struct flagName {
    ulong flag;
    const char* name;
};

#define FN(x) { x, #x }
#define FNEND { 0, 0 }

static const flagName eventCodes [] = {
    FN(evMouseDown),
    FN(evMouseUp),
    FN(evMouseMove),
    FN(evMouseAuto),
    FN(evMouseWheel),
    FN(evKeyDown),
    FN(evCommand),
    FN(evBroadcast),
    FNEND
};

static const flagName controlKeyFlags[] = {
    FN(kbShift),
    FN(kbCtrlShift),
    FN(kbAltShift),
    FN(kbScrollState),
    FN(kbNumState),
    FN(kbCapsState),
    FN(kbInsState),
    FNEND
};

static const flagName mouseEvFlags[] = {
    FN(meMouseMoved),
    FN(meDoubleClick),
    FNEND
};

static const flagName buttonFlags[] = {
    FN(mbLeftButton),
    FN(mbRightButton),
    FN(mbMiddleButton),
    FNEND
};

static const flagName wheelFlags[] = {
    FN(mwUp),
    FN(mwDown),
    FN(mwLeft),
    FN(mwRight),
    FNEND
};

static const flagName keyCodeFlags[] = {
    FN(kbCtrlA),    FN(kbCtrlB),    FN(kbCtrlC),
    FN(kbCtrlD),    FN(kbCtrlE),    FN(kbCtrlF),
    FN(kbCtrlG),    FN(kbCtrlH),    FN(kbCtrlI),
    FN(kbCtrlJ),    FN(kbCtrlK),    FN(kbCtrlL),
    FN(kbCtrlM),    FN(kbCtrlN),    FN(kbCtrlO),
    FN(kbCtrlP),    FN(kbCtrlQ),    FN(kbCtrlR),
    FN(kbCtrlS),    FN(kbCtrlT),    FN(kbCtrlU),
    FN(kbCtrlV),    FN(kbCtrlW),    FN(kbCtrlX),
    FN(kbCtrlY),    FN(kbCtrlZ),
    FN(kbEsc),      FN(kbAltSpace), FN(kbCtrlIns),
    FN(kbShiftIns), FN(kbCtrlDel),  FN(kbShiftDel),
    FN(kbBack),     FN(kbCtrlBack), FN(kbShiftTab),
    FN(kbTab),      FN(kbAltQ),     FN(kbAltW),
    FN(kbAltE),     FN(kbAltR),     FN(kbAltT),
    FN(kbAltY),     FN(kbAltU),     FN(kbAltI),
    FN(kbAltO),     FN(kbAltP),     FN(kbCtrlEnter),
    FN(kbEnter),    FN(kbAltA),     FN(kbAltS),
    FN(kbAltD),     FN(kbAltF),     FN(kbAltG),
    FN(kbAltH),     FN(kbAltJ),     FN(kbAltK),
    FN(kbAltL),     FN(kbAltZ),     FN(kbAltX),
    FN(kbAltC),     FN(kbAltV),     FN(kbAltB),
    FN(kbAltN),     FN(kbAltM),     FN(kbF1),
    FN(kbF2),       FN(kbF3),       FN(kbF4),
    FN(kbF5),       FN(kbF6),       FN(kbF7),
    FN(kbF8),       FN(kbF9),       FN(kbF10),
    FN(kbHome),     FN(kbUp),       FN(kbPgUp),
    FN(kbGrayMinus),FN(kbLeft),     FN(kbRight),
    FN(kbGrayPlus), FN(kbEnd),      FN(kbDown),
    FN(kbPgDn),     FN(kbIns),      FN(kbDel),
    FN(kbShiftF1),  FN(kbShiftF2),  FN(kbShiftF3),
    FN(kbShiftF4),  FN(kbShiftF5),  FN(kbShiftF6),
    FN(kbShiftF7),  FN(kbShiftF8),  FN(kbShiftF9),
    FN(kbShiftF10), FN(kbCtrlF1),   FN(kbCtrlF2),
    FN(kbCtrlF3),   FN(kbCtrlF4),   FN(kbCtrlF5),
    FN(kbCtrlF6),   FN(kbCtrlF7),   FN(kbCtrlF8),
    FN(kbCtrlF9),   FN(kbCtrlF10),  FN(kbAltF1),
    FN(kbAltF2),    FN(kbAltF3),    FN(kbAltF4),
    FN(kbAltF5),    FN(kbAltF6),    FN(kbAltF7),
    FN(kbAltF8),    FN(kbAltF9),    FN(kbAltF10),
    FN(kbCtrlPrtSc),FN(kbCtrlLeft), FN(kbCtrlRight),
    FN(kbCtrlEnd),  FN(kbCtrlPgDn), FN(kbCtrlHome),
    FN(kbAlt1),     FN(kbAlt2),     FN(kbAlt3),
    FN(kbAlt4),     FN(kbAlt5),     FN(kbAlt6),
    FN(kbAlt7),     FN(kbAlt8),     FN(kbAlt9),
    FN(kbAlt0),     FN(kbAltMinus), FN(kbAltEqual),
    FN(kbCtrlPgUp), FN(kbAltBack),  FN(kbNoKey),
    FN(kbCtrlUp),   FN(kbCtrlDown),
#if defined( __FLAT__ )
    FN(kbF11),      FN(kbF12),      FN(kbShiftF11),
    FN(kbShiftF12), FN(kbCtrlF11),  FN(kbCtrlF12),
    FN(kbAltF11),   FN(kbAltF12),
#endif
    FNEND
};

static void decomposeFlag(ostream &out, ulong mask, const flagName *flags, Boolean overlap = True)
{
    Boolean first = True;
    ulong found = 0;
    out << hex << "0x" << mask;
    for (; flags && flags->name; ++flags)
    {
        if (overlap ? (mask & flags->flag) : (mask == flags->flag))
        {
            if (first)
            {
                first = False;
                out << " (";
            }
            else
                out << " | ";
            out << flags->name;
            if (overlap)
                found |= mask & flags->flag;
            else
                break;
        }
    }
    if (!first)
    {
        if (overlap && (found != mask))
            out << " | 0x" << (mask & ~found);
        out << ")";
    }
    out << dec;
}

void TEventViewer::printEvent(ostream &out, const TEvent &ev)
{
    out << "TEvent {\n"
        << "  .what = ";
    if (ev.what == evNothing)
        out << "evNothing,\n";
    else
        decomposeFlag(out, ev.what, eventCodes), out << ",\n";
    if (ev.what & evMouse)
    {
        out << "  .mouse = MouseEventType {\n"
            << "    .where = TPoint {\n"
            << "      .x = " << ev.mouse.where.x << "\n"
            << "      .y = " << ev.mouse.where.y << "\n"
            << "    },\n"
            << "    .eventFlags = ";
        decomposeFlag(out, ev.mouse.eventFlags, mouseEvFlags);
        out << ",\n"
            << "    .controlKeyState = ";
        decomposeFlag(out, ev.mouse.controlKeyState, controlKeyFlags);
        out << ",\n"
            << "    .buttons = ";
        decomposeFlag(out, ev.mouse.buttons, buttonFlags);
        out << ",\n"
            << "    .wheel = ";
        decomposeFlag(out, ev.mouse.wheel, wheelFlags);
        out << "\n"
            << "  }\n";
    }
    if (ev.what & evKeyboard)
    {
        char charCode = ev.keyDown.charScan.charCode;
        out << "  .keyDown = KeyDownEvent {\n"
            << "    .keyCode = ";
        decomposeFlag(out, ev.keyDown.keyCode, keyCodeFlags, False);
        out << ",\n"
            << "    .charScan = CharScanType {\n"
            << "      .charCode = " << (int) (uchar) charCode;
        if (charCode)
            out << " ('" << charCode << "')";
        out << ",\n"
            << "      .scanCode = " << (int) (uchar) ev.keyDown.charScan.scanCode << "\n"
            << "    },\n"
            << "    .controlKeyState = ";
        decomposeFlag(out, ev.keyDown.controlKeyState, controlKeyFlags);
        out << ",\n"
            << hex
            << "    .text = {0x"
            << (int) (uchar) ev.keyDown.text[0] << ", 0x" << (int) (uchar) ev.keyDown.text[1] << ", 0x"
            << (int) (uchar) ev.keyDown.text[2] << ", 0x" << (int) (uchar) ev.keyDown.text[3] << "},\n"
            << dec
            << "    .textLength = " << (int) ev.keyDown.textLength << "\n"
            << "  }\n";
    }
    if (ev.what & evCommand)
        out << "  .message = MessageEvent {\n"
            << "    .command = " << ev.message.command << ",\n"
            << "    .infoPtr = " << ev.message.infoPtr << "\n"
            << "  }\n";
    out << "}\n";
}
