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
    u8 unknown0[0x40];
    u16 flags414, field416, field418;
    u8 unknown46[2];
    void *entryControl41C;
    struct SceneActorRegistryControl control;
    u8 unknown68[0xC];
};
struct SceneActorRegistryRoute {
    u8 unknown0[0xC];
    u8 fieldC, unknownD[3];
    u8 parameterA, parameterB, parameterC, unknown13;
};
/* Motion/reaction block embedded at actor offset 0xDC. */
struct SceneActorReactionMotion {
    u32 fieldDC;
    struct ContactPosition destination;
    u8 unknownC, heading, unknownE, flags;
};
struct SceneActorRegistryResetView {
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    u32 id;
    u8 unknown54[5];
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
    u8 unknownAC[4];
    s16 defaultHealth;
    u8 unknownB2[2];
    u16 fieldB4;
    u16 fieldB6;
    u16 fieldB8;
    u8 unknownBA[2];
    u16 fieldBC;
    u16 fieldBE;
    u16 fieldC0;
    u8 unknownC2[2];
    u16 fieldC4, fieldC6;
    u16 fieldC8;
    u8 unknownCA[2];
    u32 fieldCC;
    u32 wordD0;
    u32 wordD4;
    u8 unknownD8[4];
    u32 fieldDC;
    struct ContactPosition destinationE0;
    u8 unknownE8;
    u8 fieldE9;
    u8 unknownEA;
    u8 flagsEB;
    u8 unknownEC[4];
    u32 distanceF0, rangeF4;
    u32 distanceLimitF8; /* 080E8200: compared unsigned with distanceF0. */
    u8 unknownFC[8];
    u32 word104;
    u32 word108, word10C, word110;
    u32 flags114;
    u32 defaultFlags2CC, defaultFlags2D0; /* +0x118/+0x11C: defaults for the named live fields. */
    u8 direction120;
    u8 defaultState, defaultPhase, defaultBehavior;
    const void *defaultControl;
    u8 defaultField2B5, unknown129[3];
    u16 defaultField2EA, defaultFlags2C0;
    u32 flags130;
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
    u8 unknown220[4];
    void *entryTarget224;
    void *entryTarget228;
    void *savedTarget;
    void *reactionTarget230;
    u8 unknown234[0x14];
    void *callback248;
    u8 unknown24C[0x20];
    void *update26C;
    void *update270;
    u8 unknown274[8];
    void **callbacks27C, **callbacks280, **callbacks284;
    void **alternateBehaviorCallbacks; /* +0x288: indexed by behavior2AA. */
    void **behaviorCallbacks;          /* +0x28C: indexed by behavior2AA. */
    void **callbacks290, **callbacks294;
    void **phaseCallbacks; /* +0x298: indexed by phase2A9. */
    void **callbacks29C, **callbacks2A0;
    u8 unknown2A4[4];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, field2B3;
    u8 enabled, field2B5, field2B6, unknown2B7;
    u16 field2B8;
    u8 unknown2BA[4];
    u16 groundHeight2BE;
    u16 flags2C0, field2C2;
    u32 profileMode;
    u16 field2C8;
    u16 field2CA;
    u32 flags2CC;
    u32 flags2D0;
    u16 flags2D4;
    u16 flags2D6;
    u32 field2D8, field2DC, field2E0;
    u16 reactionSoundVariant;
    u16 field2E6;
    u8 unknown2E8[2];
    u16 field2EA;
    struct ContactPosition secondaryMotion;
    u16 terrainCell;
    u8 unknown2F6[0xE];
    u32 field304;
    u32 profileId;
    u32 field30C;
    struct ContactPosition reactionStart,
        reactionEnd; /* +0x310/+0x318: reaction position snapshots. */
    u8 unknown320[0xAC];
    u8 field3CC, unknown3CD[3];
    void *context;
    struct SceneActorRegistryControlGroup group;
    struct SceneActorRegistryRoute route;
    u8 unknown45C[4];
    u32 word460;
    u8 field464, field465, field466, field467;
    u32 field468;
    u32 delay46C; /* 080EBEE0: initialized to 60. */
    void *callback470;
    void *attachment;
    u8 unknown478[0x140];
    struct ContactPosition motion5B8;
    u16 field5C0, field5C2, field5C4, field5C6; /* 5C6: signed countdown in 08130BD4. */
    u8 unknown5C8[2];
    u8 field5CA, unknown5CB[5];
    void *routeObject;
    void *state5D4;
    u8 field5D8, field5D9, field5DA, unknown5DB;
    u8 unknown5DC[0x18];
    void *routePhaseCallback5F4;
    void *motionFailureCallback; /* +0x5F8: tentative movement failure callback. */
    u8 unknown5FC[0x28];
    u8 animation624[0x38];
    u16 animationFlags65C;
    u8 unknown65E[0x1A];
    u32 animationFlags678;
    s16 field67C;
    u8 unknown67E[4];
    s16 field682;
    u8 unknown684[4];
    void *field688;
    u8 unknown68C;
    u8 timer68D;
    u8 direction68E, unknown68F, savedDirection690;
    u8 unknown691[7];
    u16 field698;
    u16 frame69A;
    u16 animationFrame69C;
    u16 field69E;
    u8 unknown6A0[7];
    u8 animationFlag6A7;
    u8 unknown6A8[4];
    u16 field6AC;
    u8 unknown6AE[0x22];
    u16 field6D0;
    u8 unknown6D2[2];
    u16 flags6D4;
    u16 field6D6;
    struct ContactPosition vector6D8;
    struct ContactPosition vector6E0;
    u16 field6E8;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Route_Start(struct SceneActorRegistryRoute *, u32, u32, u32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
void ProximityVolume_TranslateBounds(void *, struct ContactPosition *);
#endif
