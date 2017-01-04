/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Menus.h

#ifndef _MENUS_h
#define _MENUS_h
#include "Globals.h"

extern int menuIndex;
extern vector<String> menus, menusHistory;

extern String getMenuHistory();
extern void scrollAlpha(int dir);
extern void scrollMenus(int dir);
extern void getDirectoryMenus(File directory);

#endif

