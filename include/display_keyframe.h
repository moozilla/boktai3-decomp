#ifndef GUARD_DISPLAY_KEYFRAME_H
#define GUARD_DISPLAY_KEYFRAME_H

#include "global.h"

struct DisplayKeyframeState {
    u8 unknown0[0x14];
    u16 frame;
    u8 unknown16[4];
    u8 priority;
    u8 unknown1B[2];
    u8 status;
    u8 unknown1E[0x16];
    u8 palette, paletteVariant;
    u16 attribute;
    u8 unknown38[2];
    u16 paletteIndex;
};

#endif
