#ifndef GUARD_INCLUDE_OBJECTS_FLOOR_SWITCH_FLOOR_SWITCH_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_FLOOR_SWITCH_FLOOR_SWITCH_LAYOUT_H

#include "global.h"
#include "engine/task.h"

struct FloorSwitch {
    u32 flags;
    u8 pad4[0x8];
    void *anim;
    u16 field10; /* confidence=low */
    u8 pad12[0xa];
    s16 x;
    s16 y;
    s16 z;
    u8 pad22[0xa];
    u8 animData[0x1C];
    s32 name;
    s32 r[2];
    s32 region;
    u16 sprite;
    u16 field5A; /* confidence=low */
    u8 pad5C[0x8];
    s32 s[4];
    s32 w[4];
    u16 slot; /* confidence=low */
    s16 field86; /* confidence=low: index of the update handler */
    u16 flags88;
    u8 field8A; /* confidence=low */
    u8 field8B; /* confidence=low */
    u16 field8C; /* confidence=low */
    u16 field8E; /* confidence=low */
};

typedef char FloorSwitchSizeCheck[sizeof(struct FloorSwitch) == 0x90 ? 1 : -1];

/* confidence=low */
struct FloorSwitchManager {
    struct Task task;
    struct FloorSwitch switches[8];
    u32 used; /* one bit per switch */
};

typedef char FloorSwitchManagerSizeCheck[sizeof(struct FloorSwitchManager) == 0x49c ? 1 : -1];

#endif
