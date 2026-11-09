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
    u16 flags;
    u8 pad2[2];
    u8 b4;
    u8 pad5;
    u16 w6;
};
/* A position, also copied as a pair of words. */
union SceneObjPos {
    struct Pair words;
    struct Pos16 position;
};
/* Four script argument words. The copy in SceneObjects_Append goes through the union: its stores
   may alias the pool's u16 count, so gcse leaves the count's load after the copy loop. */
union SceneObjArgs {
    s32 words[4];
    u8 bytes[16];
};
struct SceneObjSlot {
    u8 pad0[0x1c];
    union SceneObjPos spritePos; /* 0x1c: the sprite's position */
    u8 pad24[8];
    struct ObjBlock *block;
    void *callback;
    u16 id;
    u8 kind;
    u8 variant;
    u8 dir;
    u8 b39, b3a, b3b;
    u16 cellA, cellB;
    u8 type;
    u8 b41;
    u16 region; /* Region_FindContaining(position) */
    u8 b44;
    u8 b45;
    u8 pad46[2];
    s32 hasArgs; /* the script gave the four argument words */
    s32 value;
    union SceneObjArgs args;
    s32 raisedScript, loweredScript;
    union SceneObjPos pos, q0, r0; /* position, display and base positions */
    u16 lift, timer;
    u32 loweredPose;
    u16 maxLift;
    u8 pad8A[2];
};

typedef char SceneObjSlotSizeCheck[sizeof(struct SceneObjSlot) == 0x8c ? 1 : -1];

#endif
