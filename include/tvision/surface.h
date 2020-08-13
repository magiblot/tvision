/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SURFACE.H                                                             */
/*                                                                         */
/*   Defines the classes TSurface and TSurfaceView.                        */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TSurface ) && !defined( __TSurface )
#define __TSurface

// A TSurface holds a buffer of TScreenCells that can be freely written to.
// When the view draws itself, it simply displays the contents of the buffer.
// The buffer dimensions are always the same as the view's size.

class TSurface : public TView {

    friend class TSurfaceView;

    size_t areaSize;
    TScreenCell _FAR *drawArea;
    TScreenCell fill;

protected:

    void updateSize();

public:

    TSurface(const TRect &bounds);
    ~TSurface();

    virtual void draw();
    virtual void changeBounds(const TRect &bounds);

    TCellAttribs getFillColor() const;
    void setFillColor(TCellAttribs fillColor);

    void clear();

    TScreenCell _FAR &at(int y, int x);
    const TScreenCell _FAR &at(int y, int x) const;

};

inline TCellAttribs TSurface::getFillColor() const
{
    return ::getAttr(fill);
}

inline void TSurface::setFillColor(TCellAttribs fillColor)
{
    ::setAttr(fill, fillColor);
    clear();
}

inline TScreenCell _FAR &TSurface::at(int y, int x)
{
    return drawArea[y*size.x + x];
}

inline const TScreenCell _FAR &TSurface::at(int y, int x) const
{
    return drawArea[y*size.x + x];
}

#endif

#if defined( Uses_TSurfaceView ) && !defined( __TSurfaceView )
#define __TSurfaceView

// A TSurfaceView displays a region of a TSurface. The region's coordinates
// are (delta.x, delta.y), (delta.x+size.x, delta.y+size.y). Any parts of
// the region which exceed the TSurface's bounds are displayed as whitespaces.

class TSurfaceView : public TView {

protected:

    const TSurface _FAR *sface;
    TPoint delta;

public:

    TSurfaceView( const TRect &bounds,
                  const TSurface _FAR *aSface );

    virtual void draw();

    void setSurface(const TSurface _FAR *aSface);
    void setDelta(TPoint aDelta);

};

inline TSurfaceView::TSurfaceView( const TRect &bounds,
                                   const TSurface _FAR *aSface ) :
    TView(bounds),
    sface(aSface),
    delta()
{
}

inline void TSurfaceView::setSurface(const TSurface _FAR *aSface)
{
    sface = aSface;
}

inline void TSurfaceView::setDelta(TPoint aDelta)
{
    // Negative delta not supported.
    if (aDelta.x < 0)
        aDelta.x = 0;
    if (aDelta.y < 0)
        aDelta.y = 0;
    delta = aDelta;
}

#endif
