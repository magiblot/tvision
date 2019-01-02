/*------------------------------------------------------------*/
/* filename -       nmclrsel.cpp                              */
/*                                                            */
/* defines the streamable names for classes                   */
/*   TColorSelector, TMonoSelector, TColorDisplay,            */
/*   TColorGroupList, TColorItemList, TColorDialog            */
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

#define Uses_TColorSelector
#define Uses_TMonoSelector
#define Uses_TColorDisplay
#define Uses_TColorGroupList
#define Uses_TColorItemList
#define Uses_TColorDialog
#include <tv.h>

const char * const near TColorSelector::name = "TColorSelector";
const char * const near TMonoSelector::name = "TMonoSelector";
const char * const near TColorDisplay::name = "TColorDisplay";
const char * const near TColorGroupList::name = "TColorGroupList";
const char * const near TColorItemList::name = "TColorItemList";
const char * const near TColorDialog::name = "TColorDialog";


