/*------------------------------------------------------------*/
/* filename -       ttablist.cpp                             */
/*                                                            */
/* function(s)                                                */
/*                  TTableListBox member functions            */
/*------------------------------------------------------------*/

#define Uses_TTableListBox
#define Uses_TEvent
#define Uses_TText
#include <tvision/tv.h>

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

namespace {

void appendText(char *dest, short maxLen, TStringView text) noexcept
{
    size_t len = strlen(dest);
    size_t space = len < (size_t) maxLen ? (size_t) maxLen - len : 0;
    size_t count = text.size() < space ? text.size() : space;

    if( count > 0 )
        {
        memcpy(dest + len, text.begin(), count);
        dest[len + count] = EOS;
        }
}

void appendSpaces(char *dest, short maxLen, ushort count) noexcept
{
    size_t len = strlen(dest);

    while( count > 0 && len < (size_t) maxLen )
        {
        dest[len++] = ' ';
        dest[len] = EOS;
        --count;
        }
}

} // namespace

TTableListBox::TTableListBox( const TRect& bounds,
                              TScrollBar *aScrollBar ) noexcept :
    TListViewer(bounds, 1, 0, aScrollBar),
    columnPadding( 2 ),
    cells( 0 ),
    rowCount( 0 ),
    columnCount( 0 ),
    columnWidths( 0 )
{
    setRange(0);
}

TTableListBox::~TTableListBox()
{
    delete[] columnWidths;
}

ushort TTableListBox::dataSize()
{
    return sizeof(TTableListBoxRec);
}

void TTableListBox::getData( void *rec )
{
    TTableListBoxRec *p = (TTableListBoxRec *)rec;

    p->cells = cells;
    p->rowCount = rowCount;
    p->columnCount = columnCount;
    p->selection = focused;
}

void TTableListBox::getText( char *dest, short item, short maxLen )
{
    *dest = EOS;
    if( item < 0 || item >= (short) rowCount || columnCount == 0 )
        return;
    for( ushort column = 0; column < columnCount; ++column )
        {
        TStringView cell = getCell(item, column);
        ushort cellWidth = (ushort) strwidth(cell);

        appendText(dest, maxLen, cell);
        if( column + 1 < columnCount )
            {
            if( columnWidths != 0 && columnWidths[column] > cellWidth )
                appendSpaces(dest, maxLen, columnWidths[column] - cellWidth);
            appendSpaces(dest, maxLen, columnPadding);
            }
        }
}

void TTableListBox::setData( void *rec )
{
    TTableListBoxRec *p = (TTableListBoxRec *)rec;

    setRows(p->cells, p->rowCount, p->columnCount, p->selection);
}

void TTableListBox::setRows( const char * const *aCells, ushort aRowCount,
                             ushort aColumnCount, ushort selection )
{
    cells = aCells;
    rowCount = aRowCount;
    columnCount = aColumnCount;
    updateColumnWidths();
    setRange(rowCount);
    if( rowCount == 0 )
        selection = 0;
    else if( selection >= rowCount )
        selection = rowCount - 1;
    focusItem(selection);
    drawView();
}

short TTableListBox::selectedIndex() const noexcept
{
    return rowCount > 0 ? focused : -1;
}

TStringView TTableListBox::getCell( short row, short column ) const noexcept
{
    const char *cell = 0;

    if( cells != 0 && row >= 0 && row < (short) rowCount &&
        column >= 0 && column < (short) columnCount )
        cell = cells[(size_t) row * columnCount + column];
    return cell != 0 ? TStringView(cell) : TStringView();
}

void TTableListBox::updateColumnWidths()
{
    delete[] columnWidths;
    columnWidths = 0;
    if( columnCount == 0 )
        return;
    columnWidths = new ushort[columnCount];
    for( ushort column = 0; column < columnCount; ++column )
        columnWidths[column] = 0;
    for( ushort row = 0; row < rowCount; ++row )
        for( ushort column = 0; column < columnCount; ++column )
            {
            ushort width = (ushort) strwidth(getCell(row, column));
            if( width > columnWidths[column] )
                columnWidths[column] = width;
            }
}
