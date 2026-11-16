#ifndef GUARD_ROOM_ACTOR_H
#define GUARD_ROOM_ACTOR_H

#include "global.h"
#include "contact_collider.h"
#include "scene_actor_components.h"

/*
 * The object every scene-actor registry entry allocates: the room's enemies (the props,
 * such as crates and floor switches, are pool slots of their own managers). Each class
 * embeds it as its first member, so a class pointer is also a RoomActor pointer. The chase families put a struct SceneActorChaseGroup at
 * 0x3D4 right after it; the others start their own fields there. Names are provisional
 * (confidence=low) where they still carry an offset.
 */
struct RoomActor {
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x37];
    u8 movementProbe[0x10];
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
    u32 field1EC;
    u32 script1F0, script1F4, script1F8; /* script parameter words */
    s32 parameters1FC[5];
    u8 unknown210[0xC];
    void *target;
    /* State handlers (MonsterInit_InstallSequenceCallbacks): the controls the shared code
     * switches the actor to with MonsterInit_SetControl. */
    void *onHidden; /* while hidden (Vorn starts in it when field68C is set) */
    void *onEntry;  /* the two entry controls */
    void *onAlternateEntry;
    void *onStanding; /* the resting control, restored after reactions */
    void *onReaction; /* after a hit or a stimulus */
    void *onDefeat;   /* +0x234: installed by the defeat dispatch */
    void *field238, *field23C;
    void *field240;
    u8 unknown244[4];
    void *callback248;
    void *onMessage; /* the registry's messages (SceneActorRegistry_SendToCategory, ...) */
    union SceneActorCommandTable commandTable;
    u8 unknown254[4];
    void *update;  /* every frame, from SceneActorRegistry_UpdateActors */
    void *destroy; /* on removal, before the block is freed */
    union SceneActorAnimationQuerySlot animationQuery;
    void *onDamage;    /* SceneActor_UpdateActiveCallbacks, before onUpdate */
    void *onContact;   /* the contact test of the shared reaction code */
    void *targetQuery; /* resolves a pending target object */
    union SceneActorContactQuerySlot contactQuery;
    void *onUpdate; /* the class's own step (X_OnUpdate), called with the context */
    void *defaultReaction;
    void **callbacks27C, **callbacks280, **callbacks284;
    void **alternateBehaviorCallbacks; /* +0x288: indexed by behavior2AA. */
    void **behaviorCallbacks;          /* +0x28C: indexed by behavior2AA. */
    void **callbacks290, **callbacks294;
    void **phaseCallbacks; /* +0x298: indexed by phase2A9. */
    void **callbacks29C, **callbacks2A0;
    u8 unknown2A4[4];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0;
    u8 field2B1; /* set when the next phase may start; BehaviorView read it as a reset request */
    u8 field2B2; /* directional entry flag; BehaviorView read it as "prepared" */
    u8 restart;
    u8 enabled, field2B5, reactionDirection,
        animationIndex; /* reactionDirection: source direction or contact-to-contact angle (0812F520) */
    u16 field2B8;
    u8 unknown2BA[2];
    u16 defeatTimer; /* 0x2BC: set to 60 on entry to Spider_UpdateDefeat. */
    u16 groundHeight;
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
    u16 previousTerrainCell;
    u16 savedTargetCell;
    u16 regionId;
    struct ContactPosition previousPosition;
    s32 counter304; /* SceneActorBehavior_TestDefeated records a defeat while it is positive */
    u32 profileId;
    u32 field30C;
    struct ContactPosition reactionStart,
        reactionEnd; /* +0x310/+0x318: reaction position snapshots. */
    u8 shadow[0xAC]; /* +0x320: first byte gates the shadow enable/disable API. */
    u8 field3CC;
    u8 nextDirection; /* direction index (0..7) the next phase turns to */
    u8 unknown3CE[2];
    void *context;
};

#endif // GUARD_ROOM_ACTOR_H
