/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   HELPBASE.H                                                            */
/*                                                                         */
/*   defines the classes TParagraph, TCrossRef, THelpTopic, THelpIndex,    */
/*      THelpFile                                                          */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined( __HELPBASE_H )
#define __HELPBASE_H

const int32_t magicHeader = 0x46484246L; //"FBHF"

#define cHelpViewer "\x06\x07\x08"
#define cHelpWindow "\x80\x81\x82\x83\x84\x85\x86\x87"

// TParagraph

class TParagraph
{

public:

    TParagraph() {}
    TParagraph *next;
    Boolean wrap;
    ushort size;
    char *text;

};

// TCrossRef

class TCrossRef
{

public:

    TCrossRef() {}
    int ref;
    int offset;
    uchar length;

};


typedef void (*TCrossRefHandler) ( opstream&, int );

class THelpTopic: public TObject, public TStreamable
{

public:

    THelpTopic();
    THelpTopic( StreamableInit ) {};
    virtual ~THelpTopic();

    void addCrossRef( TCrossRef ref );
    void addParagraph( TParagraph *p );
    void getCrossRef( int i, TPoint& loc, uchar& length, int& ref );
    TStringView getLine( int line );
    int getNumCrossRefs();
    int numLines();
    void setCrossRef( int i, TCrossRef& ref );
    void setNumCrossRefs( int i );
    void setWidth( int aWidth );

    TParagraph *paragraphs;

    int numRefs;
    TCrossRef *crossRefs;

private:

    TStringView wrapText( char *text, int size, int& offset, Boolean wrap );
    void readParagraphs( ipstream& s );
    void readCrossRefs( ipstream& s );
    void writeParagraphs( opstream& s );
    void writeCrossRefs( opstream& s );
    void disposeParagraphs();
    const char *streamableName() const
        { return name; }
    int width;
    int lastOffset;
    int lastLine;
    TParagraph *lastParagraph;

protected:

    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const _NEAR name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, THelpTopic& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, THelpTopic*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, THelpTopic& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, THelpTopic* cl )
    { return os << (TStreamable *)cl; }


// THelpIndex

class THelpIndex : public TObject, public TStreamable
{
public:


    THelpIndex();
    THelpIndex( StreamableInit ) {};
    virtual ~THelpIndex();

    int32_t position( int );
    void add( int, int32_t );

    ushort size;
    int32_t *index;

private:

    const char *streamableName() const
        { return name; }

protected:

    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const _NEAR name;
    static TStreamable *build();

};

inline ipstream& operator >> ( ipstream& is, THelpIndex& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, THelpIndex*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, THelpIndex& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, THelpIndex* cl )
    { return os << (TStreamable *)cl; }


// THelpFile

class THelpFile : public TObject
{

    static const char * _NEAR invalidContext;

public:

    THelpFile( iopstream& s );
    virtual ~THelpFile();

    THelpTopic *getTopic( int );
    THelpTopic *invalidTopic();
    void recordPositionInIndex( int );
    void putTopic( THelpTopic* );

    iopstream *stream;
    Boolean modified;

    THelpIndex *index;
    int32_t indexPos;
};

extern void notAssigned( opstream& s, int value );

extern TCrossRefHandler crossRefHandler;

#endif  // __HELPBASE_H
