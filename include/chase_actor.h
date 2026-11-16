#ifndef GUARD_CHASE_ACTOR_H
#define GUARD_CHASE_ACTOR_H

#include "room_actor.h"
#include "scene_actor_group.h"

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
