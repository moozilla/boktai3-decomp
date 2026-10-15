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
    u8 field9E, defense9F, reactionA0;
    u8 unknownA1[0x73];
    u32 flags114;
    u8 unknown118[0x1C];
    struct ContactCollider primaryCollider, secondaryCollider;
    u16 alternatePalette, paletteBank, basePaletteBank;
    u8 unknown1E2[2];
    u16 alternateOffset, paletteOffset;
    u8 unknown1E8[8];
    u32 script1F0, script1F4, script1F8;
    s32 parameters1FC[5];
    u8 unknown210[0xA5];
    u8 kind;
    u8 unknown2B6[8];
    u16 groundHeight;
    u16 flags2C0;
    u8 unknown2C2[0xA];
    u32 flags2CC;
    u32 flags2D0;
    u16 flags2D4, flags2D6;
    s32 parameter2D8;
    u32 reaction2DC, payload2E0;
    u16 result2E4, mode2E6;
    u8 unknown2E8[0xC];
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
struct SceneActorBehaviorContactView {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[2];
    u16 kind;
    u8 unknownC[0x28];
    u32 parameter34, reactionFlags;
    u16 unknown3C, remaining, power;
    u8 unknown42, mode;
    u16 parameter44;
    u8 unknown46[0xE];
};
struct SceneActorBehaviorDestination {
    u16 flags;
    u8 unknown2[10];
    struct ContactPosition position;
    u8 unknown14[8];
    struct ContactPosition destination;
};
#endif
