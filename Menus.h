/*
*  DROMatic.ino
*  DROMatic OS Menus
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/

#ifndef _MENUS_h
#define _MENUS_h
#include "Globals.h"

extern byte menuIndex;
extern vector<String> menus, menusHistory;

extern String getMenuHistory();
extern void scrollAlpha(int dir);
extern void scrollMenus(int dir);
extern void getDirectoryMenus(File directory);

#endif

