#ifndef GUARD_INCLUDE_OBJECTS_ITEMS_RENDER_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_ITEMS_RENDER_LAYOUT_H

#include "global.h"

struct Render {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
    u8 pad12[0x2];
};

typedef char RenderSizeCheck[sizeof(struct Render) == 0x14 ? 1 : -1];

#endif
