/*------------------------------------------------------------*/
/* filename -       tchoinpl.cpp                             */
/*                                                            */
/* function(s)                                                */
/*                  TChoiceInputLine member functions         */
/*------------------------------------------------------------*/

#define Uses_TChoiceInputLine
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TListViewer
#define Uses_TScrollBar
#define Uses_TText
#define Uses_TWindow
#include <tvision/tv.h>

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#define cpChoiceWindow "\x13\x13\x15\x18\x17\x13\x14"

namespace {

TStringView textOf(const char *text) noexcept
{
    return text != 0 ? TStringView(text) : TStringView();
}

Boolean isPrintableKey(TEvent& event, char *dest, size_t maxLen) noexcept
{
    TStringView text;

    if( event.what != evKeyDown )
        return False;
    text = event.keyDown.getText();
    if( text.empty() || text.size() >= maxLen )
        return False;
    for( size_t i = 0; i < text.size(); ++i )
        {
        uchar ch = (uchar) text[i];
        if( ch < 32 || ch == 127 )
            return False;
        dest[i] = text[i];
        }
    dest[text.size()] = EOS;
    return True;
}

Boolean containsIgnoreCase(TStringView text, TStringView fragment) noexcept
{
    if( fragment.empty() )
        return True;
    if( text.size() < fragment.size() )
        return False;
    for( size_t i = 0; i + fragment.size() <= text.size(); ++i )
        if( TText::equalsIgnoreCase(text.substr(i, fragment.size()), fragment) )
            return True;
    return False;
}

int inputCursorWidth(const TInputLine& input) noexcept
{
    if( input.data == 0 )
        return 0;
    return strwidth(TStringView(input.data, input.curPos));
}

void collapseInputCursor(TInputLine& input)
{
    int rightEdge;
    int len = input.data != 0 ? strlen(input.data) : 0;

    if( input.curPos < 0 )
        input.curPos = 0;
    if( input.curPos > len )
        input.curPos = len;
    input.selStart = input.curPos;
    input.selEnd = input.curPos;
    rightEdge = inputCursorWidth(input) - input.size.x + 2;
    input.firstPos = rightEdge > 0 ? rightEdge : 0;
    input.drawView();
    input.setCursor(inputCursorWidth(input) - input.firstPos + 1, 0);
    input.showCursor();
}

Boolean fitsInputLine(const TInputLine& input, TStringView text) noexcept
{
    TTextMetrics metrics = TText::measure(text);

    return Boolean( text.size() <= (size_t) input.maxLen &&
                    metrics.width <= (uint) input.maxWidth &&
                    metrics.graphemeCount <= (uint) input.maxChars );
}

Boolean replaceInputRange(TInputLine& input, int start, int end,
    TStringView replacement)
{
    char *next;
    size_t prefixLen, suffixLen, replacementLen, nextLen;
    Boolean result = False;

    if( input.data == 0 )
        return False;
    int len = strlen(input.data);
    if( start < 0 )
        start = 0;
    if( end < start )
        end = start;
    if( end > len )
        end = len;
    prefixLen = (size_t) start;
    suffixLen = (size_t) (len - end);
    replacementLen = replacement.size();
    nextLen = prefixLen + replacementLen + suffixLen;
    if( nextLen > (size_t) input.maxLen )
        return False;
    next = new char[nextLen + 1];
    memcpy(next, input.data, prefixLen);
    memcpy(next + prefixLen, replacement.begin(), replacementLen);
    memcpy(next + prefixLen + replacementLen, input.data + end, suffixLen);
    next[nextLen] = EOS;
    if( fitsInputLine(input, next) )
        {
        strnzcpy(input.data, next, input.maxLen + 1);
        input.curPos = start + replacementLen;
        result = True;
        }
    delete[] next;
    return result;
}

Boolean applyInputKey(TInputLine& input, TEvent& event)
{
    char keyText[sizeof(event.keyDown.text) + 1];
    int start, end;

    if( input.data == 0 || event.what != evKeyDown )
        return False;
    start = min(input.selStart, input.selEnd);
    end = max(input.selStart, input.selEnd);
    if( start == end )
        start = end = input.curPos;
    if( isPrintableKey(event, keyText, sizeof(keyText)) )
        {
        if( !replaceInputRange(input, start, end, keyText) )
            return False;
        collapseInputCursor(input);
        return True;
        }
    if( ctrlToArrow(event.keyDown.keyCode) == kbBack )
        {
        if( start == end )
            {
            if( start <= 0 )
                return False;
            start -= TText::prev(input.data, start);
            }
        if( !replaceInputRange(input, start, end, TStringView()) )
            return False;
        collapseInputCursor(input);
        return True;
        }
    return False;
}

class TChoiceListViewer : public TListViewer
{
public:
    TChoiceListViewer(const TRect& bounds, TScrollBar *hScrollBar,
        TScrollBar *vScrollBar, TChoiceInputLine *aLink,
        const char * const *aChoices, short aChoiceCount,
        TStringView filter) noexcept;

    virtual void getText(char *dest, short item, short maxLen);
    virtual void handleEvent(TEvent& event);
    virtual void shutDown();
    Boolean setFilter(TStringView filter);
    const char *selectedChoice() const noexcept;
    int choiceWidth() const noexcept;

private:
    TChoiceInputLine *link;
    const char * const *choices;
    short choiceCount;
    short *items;
};

TChoiceListViewer::TChoiceListViewer(const TRect& bounds,
    TScrollBar *hScrollBar, TScrollBar *vScrollBar,
    TChoiceInputLine *aLink, const char * const *aChoices,
    short aChoiceCount, TStringView filter) noexcept :
    TListViewer(bounds, 1, hScrollBar, vScrollBar),
    link(aLink),
    choices(aChoices),
    choiceCount(aChoiceCount),
    items(0)
{
    items = new short[choiceCount > 0 ? choiceCount : 1];
    setFilter(filter);
}

void TChoiceListViewer::getText(char *dest, short item, short maxLen)
{
    const char *choice = 0;

    if( item >= 0 && item < range )
        choice = choices[items[item]];
    strnzcpy(dest, choice != 0 ? choice : "", maxLen + 1);
}

void TChoiceListViewer::handleEvent(TEvent& event)
{
    if( ((event.what == evMouseDown && (event.mouse.eventFlags & meDoubleClick)) ||
         (event.what == evKeyDown && event.keyDown.keyCode == kbEnter)) &&
        range > 0 )
        {
        endModal(cmOK);
        clearEvent(event);
        }
    else if( (event.what == evKeyDown && event.keyDown.keyCode == kbEsc) ||
             (event.what == evCommand && event.message.command == cmCancel) )
        {
        endModal(cmCancel);
        clearEvent(event);
        }
    else
        TListViewer::handleEvent(event);
}

void TChoiceListViewer::shutDown()
{
    delete[] items;
    items = 0;
    TListViewer::shutDown();
}

Boolean TChoiceListViewer::setFilter(TStringView filter)
{
    short visible = 0;

    for( short i = 0; i < choiceCount; ++i )
        if( choices[i] != 0 && link->choiceMatches(choices[i], filter) )
            items[visible++] = i;
    setRange(visible);
    focusItemNum(0);
    if( hScrollBar != 0 )
        hScrollBar->setRange(0, choiceWidth() - size.x + 3);
    drawView();
    return Boolean(visible > 0);
}

const char *TChoiceListViewer::selectedChoice() const noexcept
{
    if( focused >= 0 && focused < range )
        return choices[items[focused]];
    return 0;
}

int TChoiceListViewer::choiceWidth() const noexcept
{
    int width = 0;

    for( short i = 0; i < range; ++i )
        width = max(width, strwidth(textOf(choices[items[i]])));
    return width;
}

class TChoiceWindow : public TWindow
{
public:
    TChoiceWindow(const TRect& bounds, TChoiceInputLine *aLink,
        const char * const *choices, short choiceCount,
        TStringView filter) noexcept;

    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    const char *selectedChoice() const noexcept;

private:
    TChoiceInputLine *link;
    TChoiceListViewer *viewer;
};

TChoiceWindow::TChoiceWindow(const TRect& bounds, TChoiceInputLine *aLink,
    const char * const *choices, short choiceCount, TStringView filter) noexcept :
    TWindowInit(&TChoiceWindow::initFrame),
    TWindow(bounds, 0, wnNoNumber),
    link(aLink),
    viewer(0)
{
    TRect r = getExtent();

    flags = wfClose;
    r.grow(-1, -1);
    viewer = new TChoiceListViewer(r,
        standardScrollBar(sbHorizontal | sbHandleKeyboard),
        standardScrollBar(sbVertical | sbHandleKeyboard),
        link, choices, choiceCount, filter);
    insert(viewer);
}

TPalette& TChoiceWindow::getPalette() const
{
    static TPalette palette(cpChoiceWindow, sizeof(cpChoiceWindow) - 1);
    return palette;
}

void TChoiceWindow::handleEvent(TEvent& event)
{
    TWindow::handleEvent(event);
    if( event.what == evMouseDown && !mouseInView(event.mouse.where) )
        {
        endModal(cmCancel);
        clearEvent(event);
        }
    else if( event.what == evKeyDown && applyInputKey(*link, event) )
        {
        if( !viewer->setFilter(link->data) )
            endModal(cmCancel);
        clearEvent(event);
        }
}

const char *TChoiceWindow::selectedChoice() const noexcept
{
    return viewer != 0 ? viewer->selectedChoice() : 0;
}

} // namespace

TChoiceInputLine::TChoiceInputLine(const TRect& bounds, int limit,
    const char * const *aChoices, short aChoiceCount, TValidator *aValid,
    ushort limitMode) noexcept :
    TInputLine(bounds, limit, aValid, limitMode),
    choices(aChoices),
    choiceCount(aChoiceCount),
    maxVisibleRows(7)
{
}

void TChoiceInputLine::setChoices(const char * const *aChoices,
    short aChoiceCount) noexcept
{
    choices = aChoices;
    choiceCount = aChoiceCount;
}

Boolean TChoiceInputLine::showChoices(Boolean filtered)
{
    TChoiceWindow *choiceWindow;
    TRect r, p;
    ushort c;
    const char *selected;
    TStringView filter = filtered && data != 0 ? TStringView(data) : TStringView();
    short visibleRows = 0;

    if( owner == 0 || choices == 0 || choiceCount <= 0 )
        return False;
    for( short i = 0; i < choiceCount; ++i )
        if( choices[i] != 0 && choiceMatches(choices[i], filter) )
            ++visibleRows;
    if( visibleRows <= 0 )
        return False;
    if( maxVisibleRows > 0 && visibleRows > maxVisibleRows )
        visibleRows = maxVisibleRows;
    if( !focus() )
        return False;
    r = getBounds();
    r.a.x--;
    r.b.x++;
    r.a.y--;
    r.b.y += visibleRows + 1;
    p = owner->getExtent();
    r.intersect(p);
    r.b.y--;
    if( r.b.y - r.a.y < 3 )
        return False;
    choiceWindow = new TChoiceWindow(r, this, choices, choiceCount, filter);
    c = owner->execView(choiceWindow);
    if( c == cmOK && (selected = choiceWindow->selectedChoice()) != 0 )
        copyChoice(selected);
    else
        collapseInputCursor(*this);
    destroy(choiceWindow);
    select();
    collapseInputCursor(*this);
    return Boolean(c == cmOK);
}

void TChoiceInputLine::handleEvent(TEvent& event)
{
    char keyText[sizeof(event.keyDown.text) + 1];
    Boolean tryFilteredChoices = isPrintableKey(event, keyText, sizeof(keyText));

    if( event.what == evKeyDown &&
        ctrlToArrow(event.keyDown.keyCode) == kbDown )
        {
        showChoices(False);
        clearEvent(event);
        return;
        }
    TInputLine::handleEvent(event);
    if( tryFilteredChoices && event.what == evNothing )
        showChoices(True);
}

Boolean TChoiceInputLine::choiceMatches(TStringView choice,
    TStringView filter) const noexcept
{
    return containsIgnoreCase(choice, filter);
}

void TChoiceInputLine::copyChoice(TStringView choice)
{
    strnzcpy(data, choice, maxLen + 1);
    curPos = strlen(data);
    collapseInputCursor(*this);
}
