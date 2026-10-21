#ifndef SCENE_OBJECT_SLOT_H
#define SCENE_OBJECT_SLOT_H
#include "terrain_cell_probe.h"
/* Layout of one 0x8C-byte slot of the scene-object pool built by SceneObjects_Append (confidence=low). */
struct Pair {
    u32 a, b;
};
struct ObjSprite {
    u32 flags;
    u8 pad4[0x18];
    u32 a, b;
};
struct ObjBlock {
    u8 pad0[4];
    u8 b4;
    u8 pad5;
    u16 w6;
};
struct SceneObjSlot {
    u8 pad0[0x1c];
    u32 a, b; /* 0x1c: also sprite fields */
    u8 pad24[8];
    struct ObjBlock *block;
    void *callback;
    u8 pad34[2];
    u8 pad36;
    u8 variant;
    u8 dir;
    u8 b39, b3a, b3b;
    u16 cellA, cellB;
    u8 type;
    u8 pad41[4];
    u8 b45;
    u8 pad46[2];
    s32 pad48;
    s32 value;
    u8 pad50[0x18];
    struct Pos16 pos;
    u32 q0, q1;
    u32 r0, r1;
};

#endif
