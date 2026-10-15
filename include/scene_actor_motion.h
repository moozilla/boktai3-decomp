#ifndef GUARD_SCENE_ACTOR_MOTION_H
#define GUARD_SCENE_ACTOR_MOTION_H
#include "scene_actor_placement.h"
/* Tentative motion/tick phase of the same registered actor. */
struct SceneActorMotionTerrainGroup {
    u8 unknown0[0x1FC];
    void *contact;
};
struct SceneActorMotionSubject {
    struct ContactPosition origin;
    u8 playerIndex, unknown9[3];
    struct ContactPosition position;
};
struct SceneActorMotionRoutePoint {
    s16 x, z;
    u8 unknown4[2], attributes, unknown7;
};
struct SceneActorMotionRouteCursor {
    u8 unknown0[8];
    const struct SceneActorMotionRoutePoint *point;
    u8 fieldC, unknownD[3];
    u8 parameterA, parameterB, parameterC, unknown13;
};
struct SceneActorMotionSavedControl {
    const void *control;
    u8 state, phase, behavior, kind;
};
struct SceneActorMotionView {
    void *object, *drawable;
    u8 drawableKind, unknown9[0x47];
    u32 id;
    u8 unknown54[5], heading, unknown5A[2];
    struct ContactPosition position, velocity;
    void *motionMap;
    u8 unknown70[0x28];
    u8 category, unknown99[3];
    s16 health;
    u8 unknown9E[0x3E];
    struct SceneActorMotionSubject *subject;
    u8 unknownE0[0x24];
    u32 field104;
    u8 unknown108[0xC];
    u32 flags114;
    u8 unknown118[0x1C];
    struct ContactCollider primaryCollider, secondaryCollider;
    u16 alternatePalette, paletteBank, basePaletteBank;
    u8 unknown1E2[2];
    u16 alternateOffset, paletteOffset;
    u8 unknown1E8[0x34];
    const void *control;
    u8 unknown220[8];
    const void *alternateControl;
    u8 unknown22C[0xC];
    const void *terrainControl;
    u8 unknown23C[0x6C];
    u8 state, phase, behavior, unknown2AB;
    u32 counter;
    u8 substate, resetRequested, prepared, animationEnded;
    u8 enabled, kind, effectDirection, unknown2B7[7];
    u16 groundHeight, flags2C0;
    u16 reactionCountdown;
    u8 unknown2C4[4];
    u16 field2C8, movementTicks;
    u32 flags2CC, flags2D0;
    u16 flags2D4, flags2D6;
    s32 parameter2D8;
    u32 field2DC, payload2E0;
    u16 result2E4, mode2E6;
    u8 unknown2E8[2];
    s16 verticalOffset;
    struct ContactPosition secondaryMotion;
    u16 terrainCell, previousTerrainCell;
    u8 unknown2F8[4];
    struct ContactPosition previousPosition;
    u8 unknown304[0xD0];
    struct SceneActorRegistryControlGroup group;
    struct SceneActorMotionRouteCursor route;
    u8 unknown45C[4];
    struct SceneActorMotionSavedControl savedControl;
    u8 unknown468[0x168];
    void *terrainContact;
    struct ContactPosition *destination;
    u8 randomHeading, savedHeading, turnCountdown, unknown5DB[0x15];
    const void *turnControl;
    u8 unknown5F4[4];
    const void *failureCallback;
};
struct SceneActorMotionSequence {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[0x24];
    struct DisplaySequenceState sequence;
};
struct SceneActorMotionTile {
    u8 unknown0[0x20];
    u8 state[8];
    u32 flags;
    u16 elapsed;
    u8 unknown2E[0xF];
    u8 ended, unknown3E[2];
    struct ContactPosition position;
};
#endif
