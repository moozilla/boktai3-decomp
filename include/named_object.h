#ifndef GUARD_NAMED_OBJECT_H
#define GUARD_NAMED_OBJECT_H
#include "global.h"
#include "engine.h"
#include "ram.h"

/* Tentative: a scripted scene object (kinds 0-3) from the 12-slot pool owned by
 * gNamedActorManager. Slots are 0x19C bytes. */
struct NamedObjectArgs {
    u16 count, pad;
    u32 *values;
};
struct NamedObject {
    u8 unknown00[0xB6];
    u16 state;
    s16 flags;
    u8 stage, unknownBB;
    s16 timer;
    u16 unknownBE;
    u16 limit;
    s16 offsetC2;
    u32 enterScript;
    u32 script;
    u32 exitScript;
    u8 unknownD0[0xC];
    u32 name;
    u32 field_E0;
    u8 unknownE4[0xF];
    u8 kind;
    u8 route[4]; /* route cursor at F4: byte 2 = initial node, byte 3 = step counter */
    u8 unknownF8[0x14];
    s16 x, y, z;
    u8 unknown116[0x19C - 0x116];
};
struct NamedObjectManager {
    u8 unknown00[0x1370];
    u32 followName;
    u32 tetherName;
};
u8 sub_08101094(struct NamedObject *);
void Route_AdvancePoint(void *);
s32 Route_GetCurrentPointNodeId(void *);
u8 SceneActorRoute_IsOutsidePointBounds(void *, void *, u32, u32);
u8 NamedObject_IsPlayerWithinContactBounds(struct NamedObject *);
struct NamedObject *NamedObject_FindNearPosition(void *);
void Sound_PlaySong(u32);
#endif
