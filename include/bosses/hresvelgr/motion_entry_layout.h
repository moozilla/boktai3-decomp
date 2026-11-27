#ifndef GUARD_INCLUDE_BOSSES_HRESVELGR_MOTION_ENTRY_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_HRESVELGR_MOTION_ENTRY_LAYOUT_H

#include "global.h"
#include "scene/collision/contact_position.h"

struct MotionEntry {
    u32 flags; /* confidence=low */
    u8 pad4[0xc];
    u16 tile; /* confidence=low */
    u8 pad12[0xa];
    struct ContactPosition position;
    u8 pad24[0x8];
    u8 animation[0x10]; /* confidence=low */
    struct ContactPosition savedPosition; /* confidence=low */
    struct ContactPosition delta;
    struct ContactPosition *origin;
    u16 lifetime;
    u16 frame;
    u16 radius;
    u16 height;
    u8 type; /* confidence=low */
    u8 mode; /* confidence=low */
    u16 angularStep;
};

typedef char MotionEntrySizeCheck[sizeof(struct MotionEntry) == 0x5c ? 1 : -1];

#endif
