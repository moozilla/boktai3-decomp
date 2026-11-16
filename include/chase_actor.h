#ifndef GUARD_CHASE_ACTOR_H
#define GUARD_CHASE_ACTOR_H

#include "room_actor.h"
#include "scene_actor_group.h"

/*
 * The probe a chase actor walks its route with. Each class keeps one in its own fields,
 * followed by the route's destination; the chase group's route and destination point at
 * them, and ProximityVolume_TranslateBounds places the probe at the actor's position.
 */
struct ChaseRouteProbe {
    u16 flags, unknown2, region, polygon;
    u32 unknown8;
    struct ContactPosition origin, previous, destination;
};

/*
 * A room actor that pursues the player along the room's routes: Skeleton, Mimic, Ghoul,
 * Spider, Vorn, Golem, Mummy, Dog, Octopus, Ivy and Axe. The chase group holds the route,
 * the target placement and the pursuit control that the shared chase code drives.
 * Each of these classes embeds it first; its own fields start at 0x624.
 */
struct ChaseActor {
    struct RoomActor base;
    struct SceneActorChaseGroup group;
};

#endif // GUARD_CHASE_ACTOR_H
