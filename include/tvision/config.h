/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   CONFIG.H                                                              */
/*                                                                         */
/*   miscellaneous system-wide configuration parameters                    */
/*   FOR INTERNAL USE ONLY                                                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */

#if !defined( __CONFIG_H )
#define __CONFIG_H

#if !defined( __LIMITS_H )
#include <limits.h>
#endif  // __LIMITS_H

const eventQSize = 16;
const maxCollectionSize = (int)(( (long) UINT_MAX - 16)/sizeof( void * ));

const maxViewWidth = 132;

const maxFindStrLen    = 80;
const maxReplaceStrLen = 80;

#endif  // __CONFIG_H
