/*-----------------------------------------------------*/
/*                                                     */
/*   Turbo Vision TVHC header file                     */
/*                                                     */
/*-----------------------------------------------------*/

/* $Copyright: 1994 */

#if !defined( __TVHC_H )
#define __TVHC_H

#define Uses_fstream
#define Uses_TSortedCollection
#define Uses_TObject
#define Uses_TPoint
#include <tvision\tv.h>

#if !defined( __HELPBASE_H )
#include "tvision\helpbase.h"
#endif  // __HELPBASE_H


const MAXSIZE = 80;
const MAXSTRSIZE=256;
const MAXHELPTOPICID=16379;
const char commandChar[] = ".";
const bufferSize = 4096;

typedef enum State { undefined, wrapping, notWrapping };

class TProtectedStream : public fstream
{

public:

    TProtectedStream( char *aFileName, ushort  aMode );

private:

    char  fileName[MAXSIZE];
    ushort mode;

};

// Topic Reference

struct TFixUp
{

    long pos;
    TFixUp *next;

};

union Content
{

    uint value;
    TFixUp *fixUpList;

};

struct TReference
{

    char *topic;
    Boolean resolved;
    Content val;

};

class TRefTable : public TSortedCollection
{

public:

    TRefTable( ccIndex aLimit, ccIndex aDelta );

    virtual int compare( void *key1,void *key2 );
    virtual void freeItem( void *item );
    TReference *getReference( char *topic );
    virtual void *keyOf( void *item );

private:

    virtual void *readItem( ipstream& ) { return 0; };
    virtual void writeItem( void *, opstream& ) {};

};

struct TCrossRefNode
{
    char *topic;
    int offset;
    uchar length;
    TCrossRefNode *next;

};

class TTopicDefinition : public TObject
{

public:

    TTopicDefinition(char *aTopic, uint aValue);
    ~TTopicDefinition(void);

    char *topic;
    uint value;
    TTopicDefinition *next;

};

#endif  // __TVHC_H
