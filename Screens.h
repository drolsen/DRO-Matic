/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - July 4th, 2017
*  devin@devinrolsen.com
*/
// Screens.h

#ifndef _SCREENS_h
#define _SCREENS_h
#include "Globals.h"

extern String screenName;
extern vector<vector<vector<byte>>> matrix;
extern byte cursorX, cursorY, currentAlphaIndex, upArrow[8], downArrow[8];

extern void exitScreen();
extern void printHomeScreen();
extern void screenMatrix();
extern void printScrollArrows();
extern void printScreenNames(String menu);

#endif

