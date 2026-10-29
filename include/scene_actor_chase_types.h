#ifndef GUARD_SCENE_ACTOR_CHASE_TYPES_H
#define GUARD_SCENE_ACTOR_CHASE_TYPES_H

#include "scene_actor_reset.h"

/* Shared player, route and control records used by monster pursuit reactions.
 * Actor and monster-specific body state retain their existing declarations. */
/* A halfword flag member viewed independently of its owning actor or group.
 * Two-byte alignment also permits the actor flag member at +0x2D6. */
struct SceneActorChaseFlags16 {
    u16 value;
} __attribute__((packed, aligned(2)));

struct SceneActorChaseTarget {
    u8 unknown0[8];
    u8 playerIndex;
    u8 unknown9[3];
    struct ContactPosition position;
};

struct SceneActorChasePlayerCell {
    u16 unknown0[3];
    u16 index;
};

struct SceneActorChasePlayer {
    u8 unknown0[0x20];
    u32 flags;
    struct SceneActorChaseTarget target;
    u8 unknown38[8];
    struct SceneActorChasePlayerCell *cell;
};

struct SceneActorChaseRoutePoint {
    s16 x, z;
};

struct SceneActorChaseRouteLink {
    u8 unknown0[8];
    struct SceneActorChaseRoutePoint *point;
    u8 unknownC[8];
};

struct SceneActorChaseRoutePoints {
    u8 count, unknown1;
    u16 offset;
};

struct SceneActorChaseRouteSelection {
    u8 unknown0[2];
    u8 index, phase;
    struct SceneActorChaseRoutePoints *points;
    void *current;
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

/* Placement status at group+0x30, used by actor stability dispatch. */
struct SceneActorChasePlacement {
    u8 unknown0[4];
    u32 flags;
};

struct SceneActorChaseGroup {
    u8 unknown0[0x30];
    struct SceneActorChasePlacement placement;
    u8 unknown38[8];
    s16 flags;
    u16 field42, timer44, unknown46;
    void *homeControl;
    struct SceneActorRegistryControl control;
    u8 unknown68[0xC];
    struct SceneActorChaseRouteLink homeRoute;
    u8 unknown88[4];
    struct SceneActorChaseSavedControl saved;
    u8 unknown94[0x162];
    u8 hitReactionLatch;
    u8 unknown1F7;
    u16 effect; /* +0x1F8: environment-selected effect number. */
    u8 unknown1FA[2];
    void *route;
    struct ContactPosition *destination;
    struct SceneActorChaseTurnState turn;
    void *contactControl;
    u8 unknown20C[8];
    void *routeChoiceControl;
    u8 unknown218[4];
    void *turnControl, *routePhaseCallback, *fallback;
};

struct SceneActorChaseTracking {
    struct SceneActorChaseTarget *target;
    struct ContactPosition destination;
    u8 unknownC, heading, unknownE, flags;
};

struct SceneActorChaseTerrainCell {
    u8 attributes;
    u8 tile;
    u16 flags;
};

struct SceneActorChaseTerrainGrid {
    u32 field0;
    s16 width, height;
    s16 originX, originZ;
    struct SceneActorChaseTerrainCell cells[1];
};

struct SceneActorChaseTerrainOverride {
    u16 flags, index;
    struct SceneActorChaseTerrainCell properties;
};

struct SceneActorChaseRandomTable {
    const u16 *samples;
};

#endif
