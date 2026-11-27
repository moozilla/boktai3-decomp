#ifndef GUARD_INCLUDE_LINK_PANEL_LINK_SLOT_DISPLAY_ELEMENT_H
#define GUARD_INCLUDE_LINK_PANEL_LINK_SLOT_DISPLAY_ELEMENT_H

#include "global.h"

struct LinkSlotDisplayElement {
    u32 flags;
    u8 pad4[0x5c];
};

typedef char LinkSlotDisplayElementSizeCheck[sizeof(struct LinkSlotDisplayElement) == 0x60 ? 1 : -1];

#endif
