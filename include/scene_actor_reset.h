#ifndef GUARD_SCENE_ACTOR_RESET_H
#define GUARD_SCENE_ACTOR_RESET_H
#include "scene_actor_registry.h"
/* Extended reset-phase view; fields are offset names unless consumers establish a role. */
struct SceneActorRegistryControl {
    u8 unknown0[8];
    u16 timer;
    u8 mode, unknownB[0xD];
    u32 counter;
};
struct SceneActorRegistryControlGroup {
    u8 unknown0[0x4C];
    struct SceneActorRegistryControl control;
    u8 unknown68[0xC];
};
struct SceneActorRegistryRoute {
    u8 unknown0[0xC];
    u8 fieldC, unknownD[3];
    u8 parameterA, parameterB, parameterC, unknown13;
};
struct SceneActorRegistryResetView {
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x50];
    u8 direction;
    u8 unknown5A[0x2];
    struct ContactPosition position;
    struct ContactPosition motion;
    u8 unknown6C[0xC];
    void *currentRoute;
    u8 unknown7C[0x1C];
    u8 category;
    u8 unknown99[0x3];
    s16 health;
    u8 unknown9E[0xA];
    s16 fieldA8;
    u16 fieldAA;
    u8 unknownAC[0x58];
    u32 word104;
    u8 unknown108[0xC];
    u32 flags114;
    u8 unknown118[0x8];
    u8 direction120;
    u8 unknown121[0x13];
    struct ContactCollider colliderA;
    struct ContactCollider colliderB;
    u8 unknown1DC[0x2];
    u16 paletteBank;
    u16 basePaletteBank;
    u8 unknown1E2[0x4];
    u16 paletteOffset;
    u16 field1E8;
    u16 field1EA;
    u8 unknown1EC[0x30];
    void *target;
    u8 unknown220[0xC];
    void *savedTarget;
    u8 unknown230[0x9C];
    u32 flags2CC;
    u32 flags2D0;
    u16 flags2D4;
    u16 flags2D6;
    u8 unknown2D8[0x12];
    u16 field2EA;
    struct ContactPosition secondaryMotion;
    u16 terrainCell;
    u8 unknown2F6[0xDE];
    struct SceneActorRegistryControlGroup group;
    struct SceneActorRegistryRoute route;
    u8 unknown45C[0x174];
    void *routeObject;
};
u8 *sub_0821D64C(s32, s32);
void sub_0821E5C4(struct SceneActorRegistryRoute *, u32, u32, u32);
void sub_0821E7EC(struct ContactPosition *, u32, u32);
void sub_0821FD3C(void *, struct ContactPosition *);
#endif
