#ifndef GUARD_SCENE_ACTOR_STATE_H
#define GUARD_SCENE_ACTOR_STATE_H

#include "contact_collider.h"
#include "scene_actor_components.h"
#include "scene_actor_group.h"
#include "chase_actor.h"

/* Extended reset-phase view; fields are offset names unless consumers establish a role. */

/* Motion/reaction block embedded at actor offset 0xDC. */
struct SceneActorReactionMotion {
    u32 fieldDC;
    struct ContactPosition destination;
    u8 unknownC, heading, unknownE, flags;
};

#endif
