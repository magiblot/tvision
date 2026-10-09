/*-------------------------------------------------------------------*/
/* filename -       dosutf8.cpp                                      */
/*                                                                   */
/* function(s)                                                       */
/*          initDosUtf8 - asks the DOS for UTF-8 long file names     */
/*-------------------------------------------------------------------*/

// DOS only (16-bit Borland C++). Elsewhere this file is empty.
//
// A DOS that provides the AMIS (INT 2Dh) extension "DOS-UTF8/NAMES" can give
// and take the long file names in UTF-8: every function of INT 21h that works
// on long names (AH=71h) then returns and accepts UTF-8 for this process. This
// is what DOSBox-X does since
// https://github.com/joncampbell123/dosbox-x/pull/6632 (the option "utf8 file
// names"), and what the DOS of go2dos does.
//
// IMPORTANT: this only switches the mode of the DOS. Turbo Vision itself does
// not call the long file name functions (findfirst, fexpand, getcurdir and the
// stream classes use the Borland run-time library, which uses the short 8.3
// names of INT 21h AH=4Eh and others), and the DOS text screen and TText work
// with one byte per character in the OEM code page. The mode is therefore only
// useful for a program that calls INT 21h AH=71h itself and converts the names.
// For this reason it is off by default and enabled by the environment variable
// TV_DOS_UTF8_NAMES=1. A DOS without the provider is not affected in any way.
//
// No function here is declared 'noexcept' on purpose: this file does not
// include <tvision/tv.h>, which defines 'noexcept' away for Borland C++.
// Borland C++ 4.5 has no 'bool' type (hence the Boolean enum in
// <tvision/ttypes.h>), so the helpers return int; and, like the rest of the
// library, they are file-local through 'static' rather than an unnamed
// namespace.

#if defined( __BORLANDC__ ) && defined( __MSDOS__ ) && !defined( __FLAT__ )

#include <dos.h>
#include <stdlib.h>
#include <string.h>

static const unsigned utf8CodePage = 65001;

// Reads the 16 bytes at the real mode address seg:off.
static int readRealMode( unsigned seg, unsigned off, char *buf )
{
#if defined( __DPMI16__ )
    // DPMI function 0002h: a selector for a real mode segment.
    union REGS r;
    r.x.ax = 0x0002;
    r.x.bx = seg;
    int86( 0x31, &r, &r );
    if( r.x.cflag )
        return 0;
    const char far *p = (const char far *) MK_FP( r.x.ax, off );
#else
    const char far *p = (const char far *) MK_FP( seg, off );
#endif
    memcpy( buf, p, 16 );
    return 1;
}

// Looks for the AMIS provider whose signature (manufacturer and product, 8
// characters each) is sig. Returns the multiplex number or -1.
static int amisFind( const char *sig )
{
    for( int mux = 0; mux < 256; ++mux )
    {
        union REGS r;
        r.x.ax = (unsigned) mux << 8;       // AH = multiplex number, AL = 0: installation check
        int86( 0x2D, &r, &r );
        if( r.h.al != 0xFF )
            continue;
        char got[16];
        if( readRealMode( r.x.dx, r.x.di, got ) && memcmp( got, sig, 16 ) == 0 )
            return mux;
    }
    return -1;
}

// Function 10h: sets the encoding of this process; function 11h reads it back.
static int amisSetEncoding( int mux, unsigned encoding )
{
    union REGS r;
    r.x.ax = ((unsigned) mux << 8) | 0x10;
    r.x.bx = encoding;
    int86( 0x2D, &r, &r );
    if( r.h.al != 0xFF )
        return 0;
    r.x.ax = ((unsigned) mux << 8) | 0x11;
    int86( 0x2D, &r, &r );
    return r.h.al == 0xFF && r.x.bx == encoding;
}

void initDosUtf8()
{
    static int done = 0;
    if( done )
        return;
    done = 1;
    const char *env = getenv( "TV_DOS_UTF8_NAMES" );
    if( env == 0 || env[0] != '1' )
        return;
    static const char sig[16] =
        { 'D','O','S','-','U','T','F','8', 'N','A','M','E','S',' ',' ',' ' };
    int mux = amisFind( sig );
    if( mux >= 0 )
        amisSetEncoding( mux, utf8CodePage );
}

#else

void initDosUtf8() {}

#endif
