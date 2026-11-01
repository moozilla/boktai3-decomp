#ifndef GUARD_SCENE_ACTOR_GROUP_H
#define GUARD_SCENE_ACTOR_GROUP_H

#include "contact_position.h"

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
    u8 unknown478[0x13E];
    s16 halfHealth;
    struct ContactPosition motion5B8;
    u16 field5C0, field5C2, field5C4, field5C6;
    u16 countdown5C8;
    u8 hitReactionLatch, unknown5CB;
    u16 effect;
    u8 unknown5CE[2];
    void *route;
    struct ContactPosition *destination;
    struct SceneActorChaseTurnState turn;
    void *contactControl;
    void *field5E0, *field5E4;
    void *routeChoiceControl;
    void *field5EC;
    void *turnControl;
    void *routePhaseCallback;
    void *fallback;
    void *field5FC, *field600;
    u32 field604, field608;
    void *field60C, *field610, *field614;
    void *interactionCallback, *responseCallback;
    u32 field620;
};

#endif
