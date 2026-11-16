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
    u16 hudLabel; /* keyword 'V' (0xFFFF when absent); the HUD's label parameter (low) */
    s16 health;
    /* keyword 'A': field9E, the defense, the contact reward, then fieldA2 and fieldA4
     * (no other readers found); keyword 'X' sets fieldA1 with the experience */
    u8 field9E;
    u8 defense;       /* subtracted from a hit's power */
    u8 contactReward; /* added to the player's contact stat (PlayerStats_AddContactHundredths) */
    u8 fieldA1;       /* passed to Player_RefreshLifeState with hitAttributes */
    u16 fieldA2, fieldA4;
    s16 attachmentLimit;   /* +0xA6: signed attachment restart threshold. */
    s16 attachmentRestart; /* keyword 'l'; copied into attachmentTimer when it restarts */
    u16 attachmentTimer;
    u32 experience; /* keyword 'X'; awarded on defeat (sub_08159164) */
    s16 defaultHealth;
    u16 hudKind; /* keyword 'K' (default 2); the HUD's type (low) */
    struct SceneActorAttackParameters attacks;
    struct SceneActorChaseTracking tracking;
    u8 unknown100[4];
    /* A control queued for the next frame (MonsterInit_SetMotionControl,
     * SceneActorBehavior_ActivatePendingControl): the control, its data and the callback
     * that releases the data, the control kind, and bit 0 set while one is pending. */
    u32 pendingControl;
    u32 pendingControlData, pendingControlRelease, pendingControlKind;
    u32 pendingControlFlags;
    struct SceneActorDefaultControl saved;
    struct ContactCollider colliderA;
    struct ContactCollider colliderB;
    u16 alternatePalette; /* +0x1DC: selected by status bit 0x4000. */
    u16 paletteBank;
    u16 basePaletteBank;
    u16 paletteAlternateBank;
    u16 alternateOffset; /* +0x1E4: alternate palette offset. */
    u16 paletteOffset;
    u16 flashTimer;                      /* frames of hit flash left */
    u16 paletteTimer;                    /* frames left on a palette effect */
    u32 scriptId;                        /* keyword 'R': the script the actor runs */
    u32 script1F0, script1F4, script1F8; /* script parameter words */
    s32 parameters1FC[5];
    u8 unknown210[0xC];
    void *target;
    /* State handlers (MonsterInit_InstallSequenceCallbacks): the controls the shared code
     * switches the actor to with MonsterInit_SetControl. */
    SceneActorAction onHidden; /* while hidden (Vorn starts in it when field68C is set) */
    SceneActorAction onEntry;  /* the two entry controls */
    SceneActorAction onAlternateEntry;
    SceneActorAction onStanding; /* the resting control, restored after reactions */
    SceneActorAction onReaction; /* after a hit or a stimulus */
    SceneActorAction onDefeat;   /* +0x234: installed by the defeat dispatch */
    void *sinkCallback;          /* how the actor sinks away (SinkAndRemove, SinkThenDefeat) */
    void *reactionGate; /* the class's test before a reaction (MonsterInit_SetReactionGate) */
    void *field240;     /* a class extra step, e.g. Slime_PlayPendingSound */
    u8 unknown244[4];
    void *routeReset; /* resets the route and motion (MonsterInit_SetRouteReset) */
    SceneActorCommandHandler
        onMessage; /* the registry's messages (SceneActorRegistry_SendToCategory, ...) */
    union SceneActorCommandTable commandTable;
    u8 unknown254[4];
    SceneActorResultCallback update;  /* every frame, from SceneActorRegistry_UpdateActors */
    SceneActorResultCallback destroy; /* on removal, before the block is freed */
    union SceneActorAnimationQuerySlot animationQuery;
    void *onDamage;    /* SceneActor_UpdateActiveCallbacks, before onUpdate */
    void *onContact;   /* the contact test of the shared reaction code */
    void *targetQuery; /* resolves a pending target object */
    union SceneActorContactQuerySlot contactQuery;
    void *onUpdate; /* the class's own step (X_OnUpdate), called with the context */
    void *defaultReaction;
    void **stateCallbacks;               /* +0x27C: indexed by state2A8. */
    void **callbacks280, **callbacks284; /* tables set by X_Init; callers not yet read */
    void **alternateBehaviorCallbacks;   /* +0x288: indexed by behavior2AA. */
    void **behaviorCallbacks;            /* +0x28C: indexed by behavior2AA. */
    void **callbacks290, **callbacks294;
    void **phaseCallbacks; /* +0x298: indexed by phase2A9. */
    void **callbacks29C, **callbacks2A0;
    u8 unknown2A4[4];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 stepDone; /* set when the current control's step finishes; cleared by a new control */
    u8 field2B1; /* set when the next phase may start; BehaviorView read it as a reset request */
    u8 field2B2; /* directional entry flag; BehaviorView read it as "prepared" */
    u8 restart;
    u8 enabled, controlKind, reactionDirection,
        animationIndex; /* reactionDirection: source direction or contact-to-contact angle (0812F520) */
    u16 hitLockTimer; /* frames a contact keeps the actor locked (0x2D on a hit) */
    u8 unknown2BA[2];
    u16 defeatTimer; /* 0x2BC: set to 60 on entry to Spider_UpdateDefeat. */
    u16 groundHeight;
    u16 placementFlags; /* keyword 'M' of the placement script */
    u16 field2C2;       /* a step size, 0x20 or 0x40; readers call it speed, height or scale */
    u32 profileMode;
    u16 field2C8;     /* cleared whenever a control is installed */
    u16 frameCounter; /* counts frames; effects pulse on it modulo an interval */
    u32 behaviorFlags;
    u32 statusFlags;
    u16 motionFlags;
    u16 reactionFlags; /* reactions requested for the next frame */
    /* the last hit (SceneActorReaction_HandleColliderContact) */
    u32 hitAttributes, hitReactionFlags;
    u32 hitOwners; /* which player or source the hit came from, one bit each */
    u16 reactionSoundVariant;
    u16 field2E6;
    u16 field2E8;
    u16 heightOffset; /* height above groundHeight */
    struct ContactPosition secondaryMotion;
    u16 terrainCell;
    u16 previousTerrainCell;
    u16 savedTargetCell;
    u16 regionId;
    struct ContactPosition previousPosition;
    s32 counter304; /* SceneActorBehavior_TestDefeated records a defeat while it is positive */
    u32 profileId;
    u32 particleTimer;
    struct ContactPosition reactionStart,
        reactionEnd; /* +0x310/+0x318: reaction position snapshots. */
    u8 shadow[0xAC]; /* +0x320: first byte gates the shadow enable/disable API. */
    u8 field3CC;
    u8 nextDirection; /* direction index (0..7) the next phase turns to */
    u8 unknown3CE[2];
    void *context;
};

#endif // GUARD_ROOM_ACTOR_H
