/*------------------------------------------------------------*/
/* filename -       tmulchkb.cpp                              */
/*                                                            */
/* function(s)                                                */
/*          TMultiCheckBox member functions                   */
/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TMultiCheckBoxes
#include <tvision\tv.h>

TMultiCheckBoxes::TMultiCheckBoxes( TRect& bounds, TSItem* aStrings,
                                    uchar aSelRange, ushort aFlags,
                                    const char* aStates) :
    TCluster(bounds, aStrings)
{
    selRange = aSelRange;
    flags = aFlags;
    states = newStr(aStates);
}

#if !defined(NO_STREAMABLE)

TMultiCheckBoxes::TMultiCheckBoxes( StreamableInit ) :
    TCluster( streamableInit )
{
}

void* TMultiCheckBoxes::read( ipstream& is )
{
    TCluster::read(is);
    is >> selRange >> flags;
    states = is.readString();

    return this;
}

void TMultiCheckBoxes::write( opstream& os )
{
    TCluster::write( os );
    os << selRange << flags;
    os.writeString(states);
}

TStreamable* TMultiCheckBoxes::build()
{
    return new TMultiCheckBoxes( streamableInit );
}

#endif

TMultiCheckBoxes::~TMultiCheckBoxes()
{
    delete states;
}

void TMultiCheckBoxes::draw()
{
    drawMultiBox(" [ ] ", states);
}

ushort TMultiCheckBoxes::dataSize()
{
    return  sizeof(long);
}

uchar TMultiCheckBoxes::multiMark(int item)
{
    return (long)((value&((flags&0xff)<<(item*(flags>>8))))>>(item*(flags>>8)));
}

void TMultiCheckBoxes::getData(void* p)
{
    *(unsigned long*)p = value;
    drawView();
}

void TMultiCheckBoxes::press(int item)
{
    short curState;

    int flo = flags & 0xff;
    int fhi = flags >> 8;

    curState = (long) (value & (flo << (item*fhi))) >> (item*fhi);

    curState--;

    if ((curState >= selRange) || (curState < 0))
        curState = selRange - 1;

    value = (long)((value & ~(flo << (item*fhi))) | (curState<<(item * fhi)));
}

void TMultiCheckBoxes::setData(void* p)
{
    value = *(unsigned long*)p;
    drawView();
}

