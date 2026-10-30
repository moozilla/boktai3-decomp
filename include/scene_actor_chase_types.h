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

/* Placement status at group+0x30, used by actor stability dispatch. */

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
