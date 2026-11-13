#ifndef GUARD_SESSION_SAVE_H
#define GUARD_SESSION_SAVE_H
#include "runtime.h"
/* The 0x28-byte persistent header read by Save_ReadHeader. */
struct SessionSave {
    u32 unknown0;
    s32 solarBaseline;
    u8 unknown8[3], daylightSaving;
    s32 points;
    u32 magic;
    u16 counterA, counterB;
    u32 region;
    u8 unknown1C[0xC];
};
#endif
