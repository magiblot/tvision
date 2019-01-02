/*------------------------------------------------------------*/
/* filename -       tmnuview.cpp                              */
/*                                                            */
/* function(s)                                                */
/*                  TMenuView member functions                */
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

#define Uses_TMenuItem
#define Uses_TMenu
#define Uses_TMenuView
#define Uses_TKeys
#define Uses_TRect
#define Uses_TEvent
#define Uses_TGroup
#define Uses_TMenuBox
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#if !defined( __ASSERT_H )
#include <Assert.h>
#endif  // __ASSERT_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __STRING_H )
#include <String.h>
#endif  // __STRING_H

#define cpMenuView "\x02\x03\x04\x05\x06\x07"

TMenuItem::TMenuItem(   const char *aName,
                        ushort aCommand,
                        ushort aKeyCode,
                        ushort aHelpCtx,
                        char *p,
                        TMenuItem *aNext
             )
{
    name = newStr( aName );
    command = aCommand;
    disabled = Boolean(!TView::commandEnabled(command));
    keyCode = aKeyCode;
    helpCtx = aHelpCtx;
    if( p == 0 )
        param = 0;
    else
        param = newStr( p );
    next = aNext;
}

TMenuItem::TMenuItem( const char *aName,
                      ushort aKeyCode,
                      TMenu *aSubMenu,
                      ushort aHelpCtx,
                      TMenuItem *aNext
                    )
{
    name = newStr( aName );
    command = 0;
    disabled = Boolean(!TView::commandEnabled(command));
    keyCode = aKeyCode;
    helpCtx = aHelpCtx;
    subMenu = aSubMenu;
    next = aNext;
}

TMenuItem::~TMenuItem()
{
    delete (char *)name;
    if( command == 0 )
        delete subMenu;
    else
        delete (char *)param;
}

TMenu::~TMenu()
{
    while( items != 0 )
        {
        TMenuItem *temp = items;
        items = items->next;
        delete temp;
        }
}

void TMenuView::trackMouse( TEvent& e )
{
    TPoint mouse = makeLocal( e.mouse.where );
    for( current = menu->items; current != 0; current = current->next )
        {
        TRect r = getItemRect( current );
        if( r.contains(mouse) )
            return;
        }
}

void TMenuView::nextItem()
{
    if( (current = current->next) == 0 )
        current = menu->items;
}

void TMenuView::prevItem()
{
    TMenuItem *p;

    if( (p = current) == menu->items)
        p = 0;

    do  {
        nextItem();
        } while( current->next != p );
}

void TMenuView::trackKey( Boolean findNext )
{
    if( current == 0 )
        return;

    do  {
        if( findNext )
            nextItem();
        else
            prevItem();
        } while( current->name == 0 );
}

Boolean TMenuView::mouseInOwner( TEvent& e )
{
    if( parentMenu == 0 || parentMenu->size.y != 1 )
        return False;
    else
        {
        TPoint mouse = parentMenu->makeLocal( e.mouse.where );
        TRect r = parentMenu->getItemRect( parentMenu->current );
        return r.contains( mouse );
        }
}

Boolean TMenuView::mouseInMenus( TEvent& e )
{
    TMenuView *p =  parentMenu;
    while( p != 0 && !p->mouseInView(e.mouse.where) )
        p = p->parentMenu;

    return Boolean( p != 0 );
}

TMenuView *TMenuView::topMenu()
{
    TMenuView *p = this;
    while( p->parentMenu != 0 )
        p = p->parentMenu;
    return p;
}

enum menuAction { doNothing, doSelect, doReturn };

ushort TMenuView::execute()
{
    Boolean    autoSelect = False;
    menuAction action;
    char   ch;
    ushort result = 0;
    TMenuItem *itemShown = 0;
    TMenuItem *p;
    TMenuView *target;
    TRect  r;
    TEvent e;

    current = menu->deflt;
    do  {
        action = doNothing;
        getEvent(e);
        switch (e.what)
            {
            case  evMouseDown:
                if( mouseInView(e.mouse.where) || mouseInOwner(e) )
                    {
                    trackMouse(e);
                    if( size.y == 1 )
                        autoSelect = True;
                    }
                else
                    action =  doReturn;
                break;
            case  evMouseUp:
                trackMouse(e);
                if( mouseInOwner(e) )
                    current = menu->deflt;
                else if( current != 0 && current->name != 0 )
                    action = doSelect;
                else
                    action = doReturn;
                break;
            case  evMouseMove:
                if( e.mouse.buttons != 0 )
                    {
                    trackMouse(e);
                    if( !(mouseInView(e.mouse.where) || mouseInOwner(e)) &&
                        mouseInMenus(e) )
                        action = doReturn;
                    }
                break;
            case  evKeyDown:
                switch( ctrlToArrow(e.keyDown.keyCode) )
                    {
                    case  kbUp:
                    case  kbDown:
                        if( size.y != 1 )
                            trackKey(Boolean(ctrlToArrow(e.keyDown.keyCode) == kbDown));
                        else if( e.keyDown.keyCode == kbDown )
                            autoSelect =  True;
                        break;
                    case  kbLeft:
                    case  kbRight:
                        if( parentMenu == 0 )
                            trackKey(Boolean(ctrlToArrow(e.keyDown.keyCode) == kbRight));
                        else
                            action =  doReturn;
                        break;
                    case  kbHome:
                    case  kbEnd:
                        if( size.y != 1 )
                            {
                            current = menu->items;
                            if( e.keyDown.keyCode == kbEnd )
                                trackKey(False);
                            }
                        break;
                    case  kbEnter:
                        if( size.y == 1 )
                            autoSelect =  True;
                        action = doSelect;
                        break;
                    case  kbEsc:
                        action = doReturn;
                        if( parentMenu == 0 || parentMenu->size.y != 1 )
                            clearEvent(e);
                        break;
                    default:
                        target = this;
                        ch = getAltChar(e.keyDown.keyCode);
                        if( ch == 0 )
                            ch = e.keyDown.charScan.charCode;
                        else
                            target = topMenu();
                        p = target->findItem(ch);
                        if( p == 0 )
                            {
                            p = topMenu()->hotKey(e.keyDown.keyCode);
                            if( p != 0 && commandEnabled(p->command) )
                                {
                                result = p->command;
                                action = doReturn;
                                }
                            }
                        else if( target == this )
                            {
                            if( size.y == 1 )
                                autoSelect = True;
                            action = doSelect;
                            current = p;
                            }
                        else if( parentMenu != target ||
                                 parentMenu->current != p )
                                action = doReturn;
                    }
                break;
            case  evCommand:
                if( e.message.command == cmMenu )
                    {
                    autoSelect = False;
                    if (parentMenu != 0 )
                        action = doReturn;
                    }
                else
                    action = doReturn;
                break;
            }

        if( itemShown != current )
            {
            itemShown =  current;
            drawView();
            }

        if( (action == doSelect || (action == doNothing && autoSelect)) &&
            current != 0 &&
            current->name != 0 )
                if( current->command == 0 )
                    {
                    if( (e.what & (evMouseDown | evMouseMove)) != 0 )
                        putEvent(e);
                    r = getItemRect( current );
                    r.a.x = r.a.x + origin.x;
                    r.a.y = r.b.y + origin.y;
                    r.b = owner->size;
                    if( size.y == 1 )
                        r.a.x--;
                    target = topMenu()->newSubView(r, current->subMenu,this);
                    result = owner->execView(target);
                    destroy( target );
                    }
                else if( action == doSelect )
                    result = current->command;

        if( result != 0 && commandEnabled(result) )
            {
            action =  doReturn;
            clearEvent(e);
            }
        } while( action != doReturn );

    if( e.what != evNothing &&
        (parentMenu != 0 || e.what == evCommand))
            putEvent(e);
    if( current != 0 )
        {
        menu->deflt = current;
        current = 0;
        drawView();
        }
    return result;
}

TMenuItem *TMenuView::findItem( char ch )
{
    ch = toupper(ch);
    TMenuItem *p = menu->items;
    while( p != 0 )
        {
        if( p->name != 0 && !p->disabled )
            {
            char *loc = strchr( p->name, '~' );
            if( loc != 0 && (uchar)ch == toupper( loc[1] ) )
                return p;
            }
        p =  p->next;
        }
    return 0;
}

TRect TMenuView::getItemRect( TMenuItem * )
{
    return TRect( 0, 0, 0, 0 );
}

ushort TMenuView::getHelpCtx()
{
    TMenuView *c = this;

    while( c != 0 &&
                (c->current == 0 || 
                 c->current->helpCtx == hcNoContext ||
                 c->current->name == 0 )
         )
        c = c->parentMenu;

    if( c != 0 )
        return c->current->helpCtx;
    else
        return hcNoContext;
}

TPalette& TMenuView::getPalette() const
{
    static TPalette palette( cpMenuView, sizeof( cpMenuView )-1 );
    return palette;
}

Boolean TMenuView::updateMenu( TMenu *menu )
{
    Boolean res = False;
    for( TMenuItem *p = menu->items; p != 0; p = p->next )
        {
        if( p->name != 0 )
            if( p->command == 0 )
                {
                if (updateMenu(p->subMenu) == True)
                    res = True;
                }
            else
                {
                Boolean commandState = commandEnabled(p->command);
                if( p->disabled == commandState )
                    {
                    p->disabled = Boolean(!commandState);
                    res = True;
                    }
                }
        }
    return res;
}

void TMenuView::do_a_select( TEvent& event )
{
    putEvent( event );
    event.message.command = owner->execView(this);
    if( event.message.command != 0 && commandEnabled(event.message.command) )
        {
        event.what = evCommand;
        event.message.infoPtr = 0;
        putEvent(event);
        }
    clearEvent(event);
}

void TMenuView::handleEvent( TEvent& event )
{
    if( menu != 0 )
        switch (event.what)
            {
            case  evMouseDown:
                do_a_select(event);
                break;
            case  evKeyDown:
                if( findItem(getAltChar(event.keyDown.keyCode)) != 0 )
                    do_a_select(event);
                else
                    {
                    TMenuItem *p = hotKey(event.keyDown.keyCode);
                    if( p != 0 && commandEnabled(p->command))
                        {
                        event.what = evCommand;
                        event.message.command = p->command;
                        event.message.infoPtr = 0;
                        putEvent(event);
                        clearEvent(event);
                        }
                    }
                break;
            case  evCommand:
                if( event.message.command == cmMenu )
                    do_a_select(event);
                break;
            case  evBroadcast:
                if( event.message.command == cmCommandSetChanged )
                    {
                    if( updateMenu(menu) )
                        drawView();
                    }
                break;
            }
}


TMenuItem *TMenuView::findHotKey( TMenuItem *p, ushort keyCode )
{

    while( p != 0 )
        {
        if( p->name != 0 )
            if( p->command == 0 )
                {
                TMenuItem *T;
                if( (T = findHotKey( p->subMenu->items, keyCode )) != 0 )
                    return T;
                }
            else if( !p->disabled &&
                     p->keyCode != kbNoKey &&
                     p->keyCode == keyCode
                   )
                return p;
        p =  p->next;
        }
    return 0;
}

TMenuItem *TMenuView::hotKey( ushort keyCode )
{
    return findHotKey( menu->items, keyCode );
}

TMenuView *TMenuView::newSubView( const TRect& bounds,
                                  TMenu *aMenu,
                                  TMenuView *aParentMenu
                               )
{
    return new TMenuBox( bounds, aMenu, aParentMenu );
}

void TMenuView::writeMenu( opstream& os, TMenu *menu )
{
    uchar tok = 0xFF;
    for( TMenuItem *item = menu->items; item != 0; item = item->next )
        {
        os << tok;
        os.writeString( item->name );
        os << item->command << (int)(item->disabled)
           << item->keyCode << item->helpCtx;
        if( item->name != 0 )
            {
            if( item->command == 0 )
                writeMenu( os, item->subMenu );
            else
                os.writeString( item->param );
            }
        }
    tok = 0;
    os << tok;
}

void TMenuView::write( opstream& os )
{
    TView::write( os );
    writeMenu( os, menu );
}

TMenu *TMenuView::readMenu( ipstream& is )
{
    TMenu *menu = new TMenu;
    TMenuItem **last = &(menu->items);
    TMenuItem *item = 0;
    uchar tok;
    is >> tok;
    while( tok != 0 )
        {
        assert( tok == 0xFF );
        item = new TMenuItem( 0, 0, (void *)0 );
        *last = item;
        last = &(item->next);
        item->name = is.readString();
        int temp;
        is >> item->command >> temp
           >> item->keyCode >> item->helpCtx;
        item->disabled = Boolean( temp );
        if( item->name != 0 )
            {
            if( item->command == 0 )
                item->subMenu = readMenu( is );
            else
                item->param = is.readString();
            }
        is >> tok;
        }
    *last = 0;
    menu->deflt = menu->items;
    return menu;
}

void *TMenuView::read( ipstream& is )
{
    TView::read( is );
    menu = readMenu( is );
    parentMenu = 0;
    current = 0;
    return this;
}

TStreamable *TMenuView::build()
{
    return new TMenuView( streamableInit );
}

TMenuView::TMenuView( StreamableInit ) : TView( streamableInit )
{
}


