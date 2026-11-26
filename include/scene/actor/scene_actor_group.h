#ifndef GUARD_SCENE_ACTOR_GROUP_H
#define GUARD_SCENE_ACTOR_GROUP_H

#include "scene/actor/contact_position.h"

struct SceneActorChaseRoutePoint {
    s16 x, z;
};

struct SceneActorRegistryRoute {
    u32 unknown0, unknown4;
    struct SceneActorChaseRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};

struct SceneActorControlGate {
    s16 flags;
    u16 duration, counter, field41A;
    void *control;
};

struct SceneActorRegistryControl {
    u32 parameter : 16;
    u32 radius : 16;
    u32 radiusSquared;
    u16 timer;
    u8 mode, active;
    struct ContactPosition targetPosition;
    u16 field14;
    u8 unknown16[2];
    u32 counter;
};

struct SceneActorChasePlacement {
    u8 unknown0[4];
    u32 flags;
};

/* Header shared by group views beginning at actor +0x3D4. */
struct SceneActorRegistryControlGroup {
    u16 bodyId;
    u8 unknown2[2];
    struct ContactPosition *bodyPosition;
    u8 unknown8[0x28];
    struct SceneActorChasePlacement placement;
    u8 unknown38[8];
    struct SceneActorControlGate gate;
    struct SceneActorRegistryControl control;
    void *field43C;
    u32 field440;
    void *fallback;
};

struct SceneActorChaseSavedControl {
    void *control;
    u8 state, phase, behavior, kind;
};

struct SceneActorChaseTurnState {
    u8 direction, savedDirection;
    s8 turnsRemaining;
    u8 unknown3;
};

/* Extended pursuit group. Other species keep their distinct private tails. */
struct SceneActorChaseGroup {
    struct SceneActorRegistryControlGroup common;
    struct SceneActorRegistryRoute homeRoute;
    void *contactNode;
    struct SceneActorChaseSavedControl saved;
    u32 field468;
    u32 delay46C;
    void *callback470;
    void *attachment;
    u8 unknown478[4];
    u32 effectConfig[2];
    const struct ContactPosition *effectOrigin;
    u8 unknown488[0xE8];
    void *field570, *field574;
    u8 unknown578[0x3E];
    s16 halfHealth;
    /* 5B8..5C6: class-specific. The escalating hit keeps u16 slots at 5B8 with
     * field5C0 as their count and field5C2 as a bitmask of filled slots; Mummy clears
     * field5C4 on reset and Ghoul's states read field5C6 as a timer. */
    struct ContactPosition motion5B8;
    u16 field5C0, field5C2, field5C4, field5C6;
    u16 contactCooldown; /* set to 60 frames on a collider contact, counted down each frame */
    u8 hitReactionLatch, unknown5CB;
    u16 effect;
    u8 unknown5CE[2];
    void *route;
    struct ContactPosition *destination;
    struct SceneActorChaseTurnState turn;
    void *contactControl;
    void *animationEndControl; /* state 8, after a reaction's animation (default ResumeAfterAnimation) */
    void *routeResumeControl;  /* state 15, back onto the route (default TryResumeRoute) */
    void *routeChoiceControl;
    void *routeRestoreCallback; /* run on pending flag 0x100: each class's route or pursuit restart */
    void *turnControl;
    void *routePhaseCallback;
    void *fallback;
    void *fadeControl;        /* state 0x15 (default BeginFadedState) */
    void *defeatBurstControl; /* default BeginDefeatBurst */
    u32 animateCallback;      /* MonsterInit's `animate` argument */
    u32 field608;             /* only ever cleared */
    void *contactArrivalControl; /* state 0x19, on reaching the contact node in contact recovery */
    /* SceneActorReaction_SetMotionMode: mode 0 turns the colliders off and installs heldControl
     * (state 0x1F); mode 1 restores them, pushes the actor back and installs releaseControl
     * (state 0x20). Names are low confidence. */
    void *heldControl, *releaseControl;
    void *interactionCallback, *responseCallback;
    u32 field620; /* only ever cleared */
};

#endif
