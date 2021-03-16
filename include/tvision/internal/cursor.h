#ifndef CARET_H
#define CARET_H

#define Uses_TColorAttr
#include <tvision/tv.h>

#include <internal/buffdisp.h>

class ScreenCursor {

public:


    ScreenCursor();
    ~ScreenCursor();

    void show();
    void hide();
    bool isVisible() const;
    void setPos(const TPoint &p);
    const TPoint& getPos() const;
    void apply(TColorAttr &attr);
    void restore(TColorAttr &attr) const;

protected:

    TPoint pos;
    bool visible;
    TColorAttr backup;

    virtual void draw(TColorAttr &attr) const = 0;

};

inline ScreenCursor::ScreenCursor() :
    pos({-1, -1}),
    visible(false),
    backup(0)
{
    BufferedDisplay::addCursor(this);
}

inline ScreenCursor::~ScreenCursor()
{
    BufferedDisplay::removeCursor(this);
}

inline void ScreenCursor::show()
{
    if (!visible)
        BufferedDisplay::changeCursor();
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
        BufferedDisplay::changeCursor();
    pos = p;
}

inline const TPoint& ScreenCursor::getPos() const
{
    return pos;
}

inline void ScreenCursor::apply(TColorAttr &attr)
{
    backup = attr;
    draw(attr);
}

inline void ScreenCursor::restore(TColorAttr &attr) const
{
    attr = backup;
}

class ReverseScreenCursor : public ScreenCursor {

protected:

    void draw(TColorAttr &attr) const override;

};

class NegativeScreenCursor : public ScreenCursor {

protected:

    void draw(TColorAttr &attr) const override;

};

#endif
