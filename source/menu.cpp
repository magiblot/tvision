/*------------------------------------------------------------*/
/* filename -       menu.cpp                                  */
/*                                                            */
/* function(s)                                                */
/*                  TSubMenu member functions                 */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*                                                            */
/*    Turbo Vision -  Version 1.0                             */
/*                                                            */
/*                                                            */
/*    Copyright (c) 1991 by Borland International             */
/*    All Rights Reserved.                                    */
/*                                                            */
/*------------------------------------------------------------*/

#define Uses_TKeys
#define Uses_TSubMenu
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TMenu
#include <tv.h>

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

TSubMenu::TSubMenu( const char *nm, ushort key, ushort helpCtx ) : 
    TMenuItem( nm, 0, key, helpCtx )
{
}

TSubMenu& operator + ( TSubMenu& s, TMenuItem& i )
{
    TSubMenu *sub = &s;
    while( sub->next != 0 )
        sub = (TSubMenu *)(sub->next);

    if( sub->subMenu == 0 )
        sub->subMenu = new TMenu( i );
    else
        {
        TMenuItem *cur = sub->subMenu->items;
        while( cur->next != 0 )
            cur = cur->next;
        cur->next = &i;
        }
    return s;
}

TSubMenu& operator + ( TSubMenu& s1, TSubMenu& s2 )
{
    TMenuItem *cur = &s1;
    while( cur->next != 0 )
        cur = cur->next;
    cur->next = &s2;
    return s1;
}

TStatusDef& operator + ( TStatusDef& s1, TStatusItem& s2 )
{
    TStatusDef *def = &s1;
    while( def->next != 0 )
        def = def->next;
    if( def->items == 0 )
        def->items = &s2;
    else
        {
        TStatusItem *cur = def->items;
        while( cur->next != 0 )
            cur = cur->next;
        cur->next = &s2;
        }
    return s1;
}

TStatusDef& operator + ( TStatusDef& s1, TStatusDef& s2 )
{
    TStatusDef *cur = &s1;
    while( cur->next != 0 )
        cur = cur->next;
    cur->next = &s2;
    return s1;
}


