/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   CONFIG.H                                                              */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   miscellaneous system-wide configuration parameters                    */
/*   FOR INTERNAL USE ONLY                                                 */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if !defined( __CONFIG_H )
#define __CONFIG_H

const eventQSize = 16;
const maxCollectionSize = (int)((65536uL - 16)/sizeof( void * ));

const maxViewWidth = 132;

const maxFindStrLen    = 80;
const maxReplaceStrLen = 80;

#endif	// __CONFIG_H