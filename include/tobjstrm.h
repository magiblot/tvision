/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   TOBJSTRM.H                                                            */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TStreamable, TStreamableClass, pstream,           */
/*   ipstream, opstream, iopstream, ifpstream, ofpstream, and fpstream.    */
/*                                                                         */
/* ------------------------------------------------------------------------*/

typedef unsigned P_id_type;

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamable                                                     */
/*                                                                         */
/*   This is the base class for all storable objects.  It provides         */
/*   three member functions, streamableName(), read(), and write(), which  */
/*   must be overridden in every derived class.                            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#pragma warn -nst

#ifdef __DLL__
#define _FAR far
#else
#define _FAR
#endif

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

#if !defined( __fLink_def )
#define __fLink_def
struct fLink
{
    fLink near *f;
    class TStreamableClass near *t;
};
#endif

#define __link( s )             \
  extern TStreamableClass s;    \
  static fLink force ## s =     \
    { (fLink near *)&force ## s, (TStreamableClass near *)&s };

#if defined( Uses_TStreamable ) && !defined( __TStreamable )
#define __TStreamable

class TStreamable
{

    friend class opstream;
    friend class ipstream;

private:

    virtual const char *streamableName() const = 0;

protected:

    virtual void *read( ipstream& ) = 0;
    virtual void write( opstream& ) = 0;

};

#endif  // Uses_TStreamable

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamableClass                                                */
/*                                                                         */
/*   Used internally by TStreamableTypes and pstream.                      */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TStreamableClass ) && !defined( __TStreamableClass )
#define __TStreamableClass

#include <dos.h>
#include <limits.h>

const P_id_type P_id_notFound = UINT_MAX;

typedef TStreamable *(*BUILDER)();
#define __DELTA( d ) (FP_OFF((TStreamable *)(d *)1)-1)

class TStreamableClass
{

    friend TStreamableTypes;
    friend opstream;
    friend ipstream;

public:

    TStreamableClass( const char *n, BUILDER b, int d );

private:

    const char *name;
    BUILDER build;
    int delta;

};

#endif  // Uses_TStreamableClass

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TStreamableTypes                                                */
/*                                                                         */
/*   Maintains a database of all registered types in the application.      */
/*   Used by opstream and ipstream to find the functions to read and       */
/*   write objects.                                                        */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TStreamableTypes ) && !defined( __TStreamableTypes )
#define __TStreamableTypes

class TStreamableTypes : private TNSSortedCollection
{

public:

    TStreamableTypes();
    ~TStreamableTypes();

    void registerType( const TStreamableClass * );
    const TStreamableClass *lookup( const char * );

    void *operator new( size_t sz ) { return ::operator new( sz ); }
    void *operator new( size_t, void * );

private:

    virtual void *keyOf( void * );
    int compare( void *, void * );

};

#endif  // Uses_TStreamableTypes

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPWrittenObjects                                                */
/*                                                                         */
/*   Maintains a database of all objects that have been written to the     */
/*   current object stream.                                                */
/*                                                                         */
/*   Used by opstream when it writes a pointer onto a stream to determine  */
/*   whether the object pointed to has already been written to the stream. */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TPWrittenObjects ) && !defined( __TPWrittenObjects )
#define __TPWrittenObjects

class TPWrittenObjects : public TNSSortedCollection
{

    friend opstream;

public:

    void removeAll() { curId = 0; TNSSortedCollection::removeAll(); }

private:

    TPWrittenObjects();
    ~TPWrittenObjects();

    void registerObject( const void *adr );
    P_id_type find( const void *adr );

    void *keyOf( void * );
    int compare( void *, void * );

    P_id_type curId;

};

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPWObj                                                          */
/*                                                                         */
/*   Used internally by TPWrittenObjects.                                  */
/*                                                                         */
/* ------------------------------------------------------------------------*/

class TPWObj
{

    friend TPWrittenObjects;

private:

    TPWObj( const void *adr, P_id_type id );

    const void *address;
    P_id_type ident;

};

#endif  // Uses_TPWrittenObjects

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPReadObjects                                                   */
/*                                                                         */
/*   Maintains a database of all objects that have been read from the      */
/*   current persistent stream.                                            */
/*                                                                         */
/*   Used by ipstream when it reads a pointer from a stream to determine   */
/*   the address of the object being referred to.                          */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TPReadObjects ) && !defined( __TPReadObjects )
#define __TPReadObjects

class TPReadObjects : public TNSCollection
{

    friend ipstream;

public:

    void removeAll() { curId = 0; TNSCollection::removeAll(); }

private:

    TPReadObjects();
    ~TPReadObjects();

    void registerObject( const void *adr );
    const void *find( P_id_type id );

    P_id_type curId;

};

#endif  // Uses_TPReadObjects

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class pstream                                                         */
/*                                                                         */
/*   Base class for handling streamable objects.                           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_pstream ) && !defined( __pstream )
#define __pstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class far TStreamableTypes;

class pstream
{

    friend TStreamableTypes;

public:

    enum StreamableError { peNotRegistered, peInvalidType };
    enum PointerTypes { ptNull, ptIndexed, ptObject };

    _Cdecl pstream( streambuf _FAR * );
    virtual _Cdecl ~pstream();

    int _Cdecl rdstate() const;
    int _Cdecl eof() const;
    int _Cdecl fail() const;
    int _Cdecl bad() const;
    int _Cdecl good() const;
    void _Cdecl clear( int = 0 );
    _Cdecl operator void *() const;
    int _Cdecl operator ! () const;

    streambuf _FAR * _Cdecl rdbuf() const;

    static void initTypes();

    void _Cdecl error( StreamableError );
    void _Cdecl error( StreamableError, const TStreamable& );
    static void registerType( TStreamableClass *ts );

protected:

    _Cdecl pstream();

    streambuf _FAR *bp;
    int state;

    void _Cdecl init( streambuf _FAR * );
    void _Cdecl setstate( int );

    static TStreamableTypes * near types;

};

#endif  // Uses_pstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ipstream                                                        */
/*                                                                         */
/*   Base class for reading streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_ipstream ) && !defined( __ipstream )
#define __ipstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class far TStreamableClass;

class ipstream : virtual public pstream
{

public:

    _Cdecl ipstream( streambuf _FAR * );
    _Cdecl ~ipstream();

    streampos _Cdecl tellg();
    ipstream& _Cdecl seekg( streampos );
    ipstream& _Cdecl seekg( streamoff, ios::seek_dir );

    uchar _Cdecl readByte();
    void _Cdecl readBytes( void _FAR *, size_t );
    ushort _Cdecl readWord();
    char _FAR * _Cdecl readString();
    char _FAR * _Cdecl readString( char _FAR *, unsigned );

    friend ipstream& _Cdecl operator >> ( ipstream&, signed char& );
    friend ipstream& _Cdecl operator >> ( ipstream&, unsigned char& );
    friend ipstream& _Cdecl operator >> ( ipstream&, signed short& );
    friend ipstream& _Cdecl operator >> ( ipstream&, unsigned short& );
    friend ipstream& _Cdecl operator >> ( ipstream&, signed int& );
    friend ipstream& _Cdecl operator >> ( ipstream&, unsigned int& );
    friend ipstream& _Cdecl operator >> ( ipstream&, signed long& );
    friend ipstream& _Cdecl operator >> ( ipstream&, unsigned long& );
    friend ipstream& _Cdecl operator >> ( ipstream&, float& );
    friend ipstream& _Cdecl operator >> ( ipstream&, double& );
    friend ipstream& _Cdecl operator >> ( ipstream&, long double& );

    friend ipstream& _Cdecl operator >> ( ipstream&, TStreamable& );
    friend ipstream& _Cdecl operator >> ( ipstream&, void _FAR *& );

protected:

    _Cdecl ipstream();

    const TStreamableClass _FAR * _Cdecl readPrefix();
    void _FAR * _Cdecl readData( const TStreamableClass _FAR *,
                                        TStreamable _FAR * );
    void _Cdecl readSuffix();

    const void _FAR * _Cdecl find( P_id_type );
    void _Cdecl registerObject( const void _FAR *adr );

private:

    TPReadObjects objs;

};

#endif  // Uses_ipstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class opstream                                                        */
/*                                                                         */
/*   Base class for writing streamable objects                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_opstream ) && !defined( __opstream )
#define __opstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif


class far TStreamableClass;

class opstream : virtual public pstream
{

public:

    _Cdecl opstream( streambuf _FAR * );
    _Cdecl ~opstream();

    streampos _Cdecl tellp();
    opstream& _Cdecl seekp( streampos );
    opstream& _Cdecl seekp( streamoff, ios::seek_dir );
    opstream& _Cdecl flush();

    void _Cdecl writeByte( uchar );
    void _Cdecl writeBytes( const void _FAR *, size_t );
    void _Cdecl writeWord( ushort );
    void _Cdecl writeString( const char _FAR * );

    friend opstream& _Cdecl operator << ( opstream&, signed char );
    friend opstream& _Cdecl operator << ( opstream&, unsigned char );
    friend opstream& _Cdecl operator << ( opstream&, signed short );
    friend opstream& _Cdecl operator << ( opstream&, unsigned short );
    friend opstream& _Cdecl operator << ( opstream&, signed int );
    friend opstream& _Cdecl operator << ( opstream&, unsigned int );
    friend opstream& _Cdecl operator << ( opstream&, signed long );
    friend opstream& _Cdecl operator << ( opstream&, unsigned long );
    friend opstream& _Cdecl operator << ( opstream&, float );
    friend opstream& _Cdecl operator << ( opstream&, double );
    friend opstream& _Cdecl operator << ( opstream&, long double );

    friend opstream& _Cdecl operator << ( opstream&, TStreamable& );
    friend opstream& _Cdecl operator << ( opstream&, TStreamable _FAR * );

protected:

    _Cdecl opstream();

    void _Cdecl writePrefix( const TStreamable& );
    void _Cdecl writeData( TStreamable& );
    void _Cdecl writeSuffix( const TStreamable& );

    P_id_type _Cdecl find( const void _FAR *adr );
    void _Cdecl registerObject( const void _FAR *adr );

private:

    TPWrittenObjects *objs;

};

#endif  // Uses_opstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class iopstream                                                       */
/*                                                                         */
/*   Base class for reading and writing streamable objects                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_iopstream ) && !defined( __iopstream )
#define __iopstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class iopstream : public ipstream, public opstream
{

public:

    _Cdecl iopstream( streambuf _FAR * );
    _Cdecl ~iopstream();

protected:

    _Cdecl iopstream();

};

#endif  // Uses_iopstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpbase                                                          */
/*                                                                         */
/*   Base class for handling streamable objects on file streams            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_fpbase ) && !defined( __fpbase )
#define __fpbase

#if !defined( __FSTREAM_H )
#include <fstream.h>
#endif  // __FSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class fpbase : virtual public pstream
{

public:

    _Cdecl fpbase();
    _Cdecl fpbase( const char _FAR *, int, int = filebuf::openprot );
    _Cdecl fpbase( int );
    _Cdecl fpbase( int, char _FAR *, int );
    _Cdecl ~fpbase();

    void _Cdecl open( const char _FAR *, int, int = filebuf::openprot );
    void _Cdecl attach( int );
    void _Cdecl close();
    void _Cdecl setbuf( char _FAR *, int );
    filebuf _FAR * _Cdecl rdbuf();

private:

    filebuf buf;

};

#endif  // Uses_fpbase

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ifpstream                                                       */
/*                                                                         */
/*   Base class for reading streamable objects from file streams           */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_ifpstream ) && !defined( __ifpstream )
#define __ifpstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class ifpstream : public fpbase, public ipstream
{

public:

    _Cdecl ifpstream();
    _Cdecl ifpstream( const char _FAR *,
                      int = ios::in,
                      int = filebuf::openprot
                    );
    _Cdecl ifpstream( int );
    _Cdecl ifpstream( int, char _FAR *, int );
    _Cdecl ~ifpstream();

    filebuf _FAR * _Cdecl rdbuf();
    void _Cdecl open( const char _FAR *,
                      int = ios::in,
                      int = filebuf::openprot
                    );

};

#endif  // Uses_ifpstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ofpstream                                                       */
/*                                                                         */
/*   Base class for writing streamable objects to file streams             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_ofpstream ) && !defined( __ofpstream )
#define __ofpstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif


class ofpstream : public fpbase, public opstream
{

public:

    _Cdecl ofpstream();
    _Cdecl ofpstream( const char _FAR *,
                      int = ios::out,
                      int = filebuf::openprot
                    );
    _Cdecl ofpstream( int );
    _Cdecl ofpstream( int, char _FAR *, int );
    _Cdecl ~ofpstream();

    filebuf _FAR * _Cdecl rdbuf();
    void _Cdecl open( const char _FAR *,
                      int = ios::out,
                      int = filebuf::openprot
                    );

};

#endif  // Uses_ofpstream

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpstream                                                        */
/*                                                                         */
/*   Base class for reading and writing streamable objects to              */
/*   bidirectional file streams                                            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_fpstream ) && !defined( __fpstream )
#define __fpstream

#if !defined( __IOSTREAM_H )
#include <iostream.h>
#endif  // __IOSTREAM_H

#pragma option -Vo-
#if defined( __BCOPT__ )
#pragma option -po-
#endif

class fpstream : public fpbase, public iopstream
{

public:

    _Cdecl fpstream();
    _Cdecl fpstream( const char _FAR *, int, int = filebuf::openprot );
    _Cdecl fpstream( int );
    _Cdecl fpstream( int, char _FAR *, int );
    _Cdecl ~fpstream();

    filebuf _FAR * _Cdecl rdbuf();
    void _Cdecl open( const char _FAR *, int, int = filebuf::openprot );

};


#endif  // Uses_fpstream

#pragma option -Vo.
#if defined( __BCOPT__ )
#pragma option -po.
#endif
