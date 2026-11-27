#ifndef GUARD_INCLUDE_UI_PAUSE_MAP_MENU_H
#define GUARD_INCLUDE_UI_PAUSE_MAP_MENU_H

#include "global.h"

struct MapMenuMarker;
struct MapMenuPage;

struct MapMenu {
    u8 f0[0x20]; /* confidence=low */
    u32 arg;
    u8 pad24[0x3804];
    u32 flagsA;
    u8 pad382C[0x14];
    s16 posAX; /* confidence=low */
    s16 posAY; /* confidence=low */
    u8 pad3844[0x44];
    u32 flagsB;
    u8 pad388C[0x14];
    s16 posBX; /* confidence=low */
    s16 posBY; /* confidence=low */
    u8 pad38A4[0x1054];
    struct MapMenuPage *pages; /* confidence=low */
    u8 pad48FC[0x4];
    struct MapMenuMarker *markers;
    u8 index; /* confidence=low */
    u8 unlocked; /* confidence=low */
    u8 pad4906[0x2];
    s8 adjacent[4]; /* confidence=low */
    u16 scrollX;
    u16 scrollY;
    s16 minScrollX; /* confidence=low */
    s16 minScrollY; /* confidence=low */
    s16 maxScrollX; /* confidence=low */
    s16 maxScrollY; /* confidence=low */
    s16 focusX; /* confidence=low */
    s16 focusY; /* confidence=low */
    u16 map;
    u16 x;
    u16 y;
    u16 otherMap;
    u16 otherX;
    u16 otherY;
    u32 hideSpecialMarkers;
};

typedef char MapMenuSizeCheck[sizeof(struct MapMenu) == 0x492c ? 1 : -1];

#endif
