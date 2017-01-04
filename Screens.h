/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/
// Screens.h

#ifndef _SCREENS_h
#define _SCREENS_h
#include "Globals.h"

extern String screenName;
extern vector<vector<vector<byte>>> matrix;
extern int cursorX, cursorY, currentAlphaIndex;
extern byte upArrow[8], downArrow[8];

extern void exitScreen();
extern void openHomeScreen();
extern void screenMatrix();
extern void printScrollArrows();
extern void printDisplayNames(String menu);

#endif

