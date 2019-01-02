/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   HELPBASE.H                                                            */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TParagraph, TCrossRef, THelpTopic, THelpIndex,    */
/*      THelpFile                                                          */
/*                                                                         */
/* ------------------------------------------------------------------------*/

const long magicHeader = 0x46484246L; //"FBHF"

#define cHelpColor      "\x37\x3F\x3A\x13\x13\x30\x3E\x1E"
#define cHelpBlackWhite "\x07\x0F\x07\x70\x70\x07\x0F\x70" 
#define cHelpMonochrome "\x07\x0F\x07\x70\x70\x07\x0F\x70"
#define cHelpViewer     "\x06\x07\x08"
#define cHelpWindow     "\x40\x41\x42\x43\x44\x45\x46\x47"

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
    char *getLine( int line );
    int getNumCrossRefs();
    int numLines();
    void setCrossRef( int i, TCrossRef& ref );
    void setNumCrossRefs( int i );
    void setWidth( int aWidth );

    TParagraph *paragraphs;

    int numRefs;
    TCrossRef *crossRefs;

private:

    char *wrapText( char *text, int size, int& offset, Boolean wrap );
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

    static const char * const near name;
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

    long position( int );
    void add( int, long );

    ushort size;
    long *index;

private:

    const char *streamableName() const
        { return name; }

protected:

    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const near name;
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

public:

    THelpFile( fpstream& s );
    virtual ~THelpFile();

    THelpTopic *getTopic( int );
    THelpTopic *invalidTopic();
    void recordPositionInIndex( int );
    void putTopic( THelpTopic* );

    fpstream *stream;
    Boolean modified;

    THelpIndex *index;
    long indexPos;
};

extern void notAssigned( opstream& s, int value );

extern TCrossRefHandler crossRefHandler;

