#ifndef GUARD_TEXT_ENTRY_WIDGET_H
#define GUARD_TEXT_ENTRY_WIDGET_H

#include "global.h"

struct TextEntryWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x20 - 0xC];
    s16 x, y;
    u8 unknown24[0x3a - 0x24];
    u16 paletteBank;
    u8 unknown3C[0x48 - 0x3c];
    u16 *palette;
    u8 unknown4C[0x60 - 0x4c];
    u8 resource[0x20], unknown80[8];
};

#endif
