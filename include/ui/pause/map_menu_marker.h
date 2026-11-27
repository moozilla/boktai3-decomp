#ifndef GUARD_INCLUDE_UI_PAUSE_MAP_MENU_MARKER_H
#define GUARD_INCLUDE_UI_PAUSE_MAP_MENU_MARKER_H

#include "global.h"

struct MapMenuMarkerSprite {
    u8 pad0[0x8];
    u32 flags;
    u8 padC[0x14];
    u16 x;
    u16 y;
    u16 z;
    u8 pad26[0x3a];
};

typedef char MapMenuMarkerSpriteSizeCheck[sizeof(struct MapMenuMarkerSprite) == 0x60 ? 1 : -1];

#endif
