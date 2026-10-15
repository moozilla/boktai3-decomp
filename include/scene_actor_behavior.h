#ifndef GUARD_SCENE_ACTOR_BEHAVIOR_H
#define GUARD_SCENE_ACTOR_BEHAVIOR_H
#include "scene_actor_registry.h"
/* Tentative view shared by the behavior helpers beginning at08078078. */
struct SceneActorBehaviorView {
    void *object, *drawable;
    u8 drawableKind, unknown9[0x50];
    u8 heading, unknown5A[2];
    struct ContactPosition position, velocity;
    u8 unknown6C[0x2C];
    u8 category, unknown99[3];
    s16 health;
    u8 unknown9E[0x76];
    u32 flags114;
    u8 unknown118[0xCE];
    u16 paletteOffset;
    u8 unknown1E8[8];
    u32 script1F0, script1F4, script1F8;
    s32 parameters1FC[5];
    u8 unknown210[0xAE];
    u16 groundHeight;
    u8 unknown2C0[0x10];
    u32 flags2D0;
    u16 flags2D4, flags2D6;
    s32 parameter2D8;
    u8 unknown2DC[0x18];
    u16 terrainCell;
    u8 unknown2F6[0xE];
    s32 counter304;
    u8 unknown308[8];
    u16 biasX310;
    u8 unknown312[2];
    u16 biasZ314;
    u8 unknown316[0xB7];
    u8 requestedDirection;
};
struct SceneActorBehaviorDestination {
    u16 flags;
    u8 unknown2[10];
    struct ContactPosition position;
    u8 unknown14[8];
    struct ContactPosition destination;
};
#endif
