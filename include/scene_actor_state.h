#ifndef GUARD_SCENE_ACTOR_STATE_H
#define GUARD_SCENE_ACTOR_STATE_H

#include "contact_collider.h"
#include "scene_actor_components.h"

/* Extended reset-phase view; fields are offset names unless consumers establish a role. */
struct SceneActorRegistryControl {
    u8 unknown0[8];
    u16 timer;
    u8 mode, unknownB[9];
    u16 field14; /* +0x14: cleared with the route timer/counter. */
    u8 unknown16[2];
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
    u8 unknown0[8];
    const u16 *point; /* +0x8: active route point; 08078EE8. */
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
    struct SceneActorBody body;
    u8 category;
    u8 unknown99;
    u16 field9A;
    s16 health;
    u8 field9E, field9F, fieldA0, fieldA1;
    u16 fieldA2, fieldA4;
    s16 attachmentLimit; /* +0xA6: signed attachment restart threshold. */
    s16 fieldA8;
    u16 fieldAA;
    u32 fieldAC;
    s16 defaultHealth;
    u16 fieldB2;
    struct SceneActorAttackParameters attacks;
    struct SceneActorChaseTracking tracking;
    u8 unknown100[4];
    u32 word104;
    u32 word108, word10C, word110;
    u32 flags114;
    struct SceneActorDefaultControl saved;
    struct ContactCollider colliderA;
    struct ContactCollider colliderB;
    u16 alternatePalette; /* +0x1DC: selected by status bit 0x4000. */
    u16 paletteBank;
    u16 basePaletteBank;
    u16 paletteAlternateBank;
    u16 alternateOffset; /* +0x1E4: alternate palette offset. */
    u16 paletteOffset;
    u16 field1E8;
    u16 field1EA;
    u32 field1EC, field1F0;
    u8 unknown1F4[0x28];
    void *target;
    void *field220;
    void *entryTarget224;
    void *entryTarget228;
    void *savedTarget;
    void *reactionTarget230;
    void *deathControl; /* +0x234: controller installed by defeat dispatch. */
    void *field238, *field23C;
    u32 field240;
    u8 unknown244[4];
    void *callback248;
    void *messageCallback;
    union SceneActorCommandTable commandTable;
    u8 unknown254[4];
    void *updateCallback, *destroyCallback;
    union SceneActorAnimationQuerySlot animationQuery;
    void *damageCallback, *contactCallback;
    void *update26C;
    union SceneActorContactQuerySlot contactQuery;
    void *contextCallback, *defaultReaction;
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
    u8 enabled, field2B5, field2B6, animationIndex;
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
    u16 field2E8;
    u16 field2EA;
    struct ContactPosition secondaryMotion;
    u16 terrainCell;
    u8 unknown2F6[4];
    u16 regionId;
    u8 unknown2FC[8];
    u32 field304;
    u32 profileId;
    u32 field30C;
    struct ContactPosition reactionStart,
        reactionEnd; /* +0x310/+0x318: reaction position snapshots. */
    u8 shadow[0xAC]; /* +0x320: first byte gates the shadow enable/disable API. */
    u8 field3CC, unknown3CD[3];
    void *context;
    struct SceneActorRegistryControlGroup group;
    struct SceneActorRegistryRoute route;
    void *contactNode; /* +0x45C: passed to SceneActorContacts_ReleaseNode. */
    u32 word460;
    u8 field464, field465, field466, field467;
    u32 field468;
    u32 delay46C; /* 080EBEE0: initialized to 60. */
    void *callback470;
    void *attachment;
    u8 unknown478[0x140];
    struct ContactPosition motion5B8;
    u16 field5C0, field5C2, field5C4, field5C6; /* 5C6: signed countdown in 08130BD4. */
    u16 countdown5C8;
    u8 field5CA, unknown5CB;
    u16 effect5CC;
    u8 unknown5CE[2];
    void *routeObject;
    void *state5D4;
    u8 field5D8, field5D9, field5DA, unknown5DB;
    void *behaviorControl;
    u8 unknown5E0[0x14];
    void *routePhaseCallback5F4;
    void *motionFailureCallback; /* +0x5F8: tentative movement failure callback. */
    u8 unknown5FC[0x1C];
    void *interactionCallback; /* +0x618: invoked when tether interaction ends. */
    void *responseCallback;    /* +0x61C: response-slot continuation. */
    u8 unknown620[4];
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

#endif
