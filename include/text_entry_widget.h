#ifndef GUARD_TEXT_ENTRY_WIDGET_H
#define GUARD_TEXT_ENTRY_WIDGET_H

#include "global.h"

struct TextEntryWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x20 - 0xC];
    s16 x, y;
    u8 unknown24[0x88 - 0x24];
};

#endif
