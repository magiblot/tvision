#ifndef CARET_H
#define CARET_H

#include <internal/buffdisp.h>

class ScreenCursor {

public:

    enum DrawStyle { Reverse, Negative };

    ScreenCursor(DrawStyle);
    ~ScreenCursor();

    void show();
    void hide();
    bool isVisible() const;
    void setPos(const TPoint &p);
    const TPoint& getPos() const;
    void apply(uchar &attr);
    void restore(uchar &attr) const;

protected:

    const DrawStyle style;
    TPoint pos;
    bool visible;
    uchar backup;

    virtual void draw(uchar &attr) const;

};

inline ScreenCursor::ScreenCursor(DrawStyle style) :
    style(style),
    pos({-1, -1}),
    visible(false),
    backup(0)
{
    BufferedDisplay::cursors.insert(this);
}

inline ScreenCursor::~ScreenCursor()
{
    BufferedDisplay::cursors.erase(this);
}

inline void ScreenCursor::show()
{
    if (!visible)
        BufferedDisplay::instance->caretMoved = true;
    visible = true;
}

inline void ScreenCursor::hide()
{
    visible = false;
}

inline bool ScreenCursor::isVisible() const
{
    return visible;
}

inline void ScreenCursor::setPos(const TPoint &p)
{
    if (visible && p != pos)
        BufferedDisplay::instance->caretMoved = true;
    pos = p;
}

inline const TPoint& ScreenCursor::getPos() const
{
    return pos;
}

inline void ScreenCursor::apply(uchar &attr)
{
    backup = attr;
    draw(attr);
}

inline void ScreenCursor::restore(uchar &attr) const
{
    attr = backup;
}

#endif
