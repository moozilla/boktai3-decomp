#ifndef GUARD_SESSION_SAVE_H
#define GUARD_SESSION_SAVE_H
#include "engine/runtime.h"
/* The 0x28-byte persistent header read by Save_ReadHeader. */
struct SessionSave {
    u32 unknown0;
    s32 solarBaseline;
    u8 selectedSlot, saveRequested, unknownA, daylightSaving;
    s32 points;
    u32 magic;
    u16 counterA, counterB;
    u32 region;
    u8 unknown1C[0xC];
};

typedef char SessionSaveSizeCheck[sizeof(struct SessionSave) == 0x28 ? 1 : -1];

#endif
