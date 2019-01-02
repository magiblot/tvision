/*--------------------------------------------------------------*/
/* filename -       tresfile.cpp                                */
/*                                                              */
/* function(s)                                                  */
/*                  TResourceFile member functions              */
/*--------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#define Uses_TResourceFile
#define Uses_TResourceItem
#define Uses_fpstream
#define Uses_TCollection
#define Uses_TStringCollection
#define Uses_TResourceCollection
#include <tvision\tv.h>

#if !defined( __IO_H )
#include <io.h>
#endif  // __IO_H

const long rStreamMagic = 0x52504246uL; // 'FBPR'

struct Count_type
{
    ushort lastCount;
    ushort pageCount;
};

struct Info_type
{
    ushort infoType;
    long infoSize;
};

struct THeader
{
    ushort signature;
    union
        {
        Count_type count;
        Info_type info;
        };
};


TResourceFile::TResourceFile( fpstream *aStream ) : TObject()
{
    THeader *header;
    int handle;
    int found;
    int repeat;
    long streamSize;

    stream = aStream;
    basePos = stream->tellp();
    handle = stream->rdbuf()->fd();
    streamSize = filelength(handle);
    header = new THeader;
    found = 0;
    do {
       repeat = 0;
       if (basePos <= (streamSize - sizeof(THeader)))
           {
           stream->seekg(basePos, ios::beg);
           stream->readBytes(header, sizeof(THeader));
           if (header->signature == 0x5a4d)
               {
               basePos += ((header->count.pageCount * 512L) -
                          (-header->count.lastCount & 511));
               repeat = 1;
               }
           else if (header->signature == 0x4246)
               {
               if (header->info.infoType == 0x5250)
                   found = 1;
               else
                   {
                   basePos +=
                      header->info.infoSize + 16 - (header->info.infoSize)%16;
                   repeat = 1;
                   }
               }
           }
        } while (repeat);

    delete header;

    if (found)
    {
        stream->seekg(basePos + sizeof(long) * 2, ios::beg);
        *stream >> indexPos;
        stream->seekg(basePos + indexPos, ios::beg);
        *stream >> index;
    }
    else
    {
        indexPos =  sizeof(long) * 3;
        index = new TResourceCollection(0, 8);
    }
}

TResourceFile::~TResourceFile()
{
    flush();
    destroy( (TCollection *)index );
    delete stream;
}

short TResourceFile::count()
{
    return index->getCount();
}

void TResourceFile::remove( const char *key )
{
    int i;

    if (index->search( (char *)key, i))
        {
        index->free(index->at(i));
        modified = True;
        }
}

void TResourceFile::flush()
{
    long lenRez;

    if (modified == True)
    {
        stream->seekp(basePos + indexPos, ios::beg);
        *stream << index;
        lenRez =  stream->tellp() - basePos -  sizeof(long) * 2;
        stream->seekp(basePos, ios::beg);
        *stream << rStreamMagic;
        *stream << lenRez;
        *stream << indexPos;
        stream->flush();
        modified = False;
    }
}

void *TResourceFile::get( const char *key)
{
    int i;
    void *p;

    if (! index->search((char *)key, i))
        return  0;
    stream->seekg(basePos + ((TResourceItem*)(index->at(i)))->pos, ios::beg);
    *stream >> p;
    return p;
}

const char *TResourceFile::keyAt(short i)
{
    return ((TResourceItem*)(index->at(i)))->key;
}

void TResourceFile::put(TStreamable *item, const char *key)
{
    int i;
    TResourceItem  *p;

    if (index->search( (char *)key, i))
        p = (TResourceItem*)(index->at(i));
    else
    {
        p = new TResourceItem;
        p->key = newStr(key);
        index->atInsert(i, p);
    }
    p->pos =  indexPos;
    stream->seekp(basePos + indexPos, ios::beg);
    *stream << item;
    indexPos = stream->tellp() - basePos;
    p->size  = indexPos - p->pos;

    modified = True;
}

void copyStream( fpstream* dest, fpstream* src, long n)
{
	const xferSize=256;

	char *xferBuf = new char[xferSize];
	size_t thisMove;

	while (n > 0)
	{
		if (n > xferSize)
			thisMove = xferSize;
		else
			thisMove = (int)n;

		src->readBytes(xferBuf, thisMove);
		dest->writeBytes(xferBuf, thisMove);
		n -= thisMove;
	}

	delete xferBuf;
}

struct SwitchInfo
{
	fpstream* sourceStream;
	fpstream* destStream;
	long oldBasePos;
	long newBasePos;
};

void doCopyResource(void* item, void* arg)
{
  SwitchInfo* si = (SwitchInfo*)arg;

  si->sourceStream->seekg(si->oldBasePos + ((TResourceItem*)item)->pos);
  ((TResourceItem*)item)->pos = si->destStream->tellp() - si->newBasePos;

  copyStream( si->destStream, si->sourceStream, ((TResourceItem*)item)->size);
}

fpstream* TResourceFile::switchTo( fpstream *aStream, Boolean pack )
{
  SwitchInfo args;

  args.newBasePos = aStream->tellp();
  args.oldBasePos = basePos;

  if (pack)
  {
  	 args.sourceStream = stream;
	 args.destStream = aStream;
    aStream->seekp( args.newBasePos + sizeof(long)*3);
    index->forEach(doCopyResource, &args);
    indexPos = aStream->tellp() - args.newBasePos;
  }
  else
  {
    stream->seekg(basePos);
	 copyStream(aStream, stream, indexPos);
  }

  modified = True;
  basePos = args.newBasePos;

  fpstream* oldStream = stream;
  stream = aStream;

  return oldStream;
}

