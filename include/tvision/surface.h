/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SURFACE.H                                                             */
/*                                                                         */
/*   Defines the classes TDrawSurface and TSurfaceView.                    */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TDrawSurface ) && !defined( __TDrawSurface )
#define __TDrawSurface

// A TDrawSurface holds a two-dimensional buffer of TScreenCells
// that can be freely written to.

class TDrawSurface
{

    size_t dataLength;
    TScreenCell _FAR *data;

public:

    TPoint size;

    TDrawSurface();
    TDrawSurface(TPoint aSize);
    ~TDrawSurface();

    void resize(TPoint aSize);
    void grow(TPoint aDelta);
    void clear();

    // Warning: no bounds checking.
    TScreenCell _FAR &at(int y, int x);
    const TScreenCell _FAR &at(int y, int x) const;

};

inline void TDrawSurface::grow(TPoint aDelta)
{
    resize(size + aDelta);
}

inline TScreenCell _FAR &TDrawSurface::at(int y, int x)
{
    return data[y*size.x + x];
}

inline const TScreenCell _FAR &TDrawSurface::at(int y, int x) const
{
    return data[y*size.x + x];
}

#endif

#if defined( Uses_TSurfaceView ) && !defined( __TSurfaceView )
#define __TSurfaceView

// A TSurfaceView displays a region of a TDrawSurface with coordinates
// (delta.x, delta.y), (delta.x+size.x, delta.y+size.y). Any parts of the region
// which exceed the TDrawSurface's bounds are displayed as whitespaces.

class TSurfaceView : public TView
{

public:

    const TDrawSurface _FAR *sface;
    TPoint delta;
    TColorAttr fillColor;

    TSurfaceView( const TRect &bounds,
                  const TDrawSurface _FAR *aSface=0,
                  TColorAttr aColor=TColorAttr() );

    virtual void draw();

};

#endif
