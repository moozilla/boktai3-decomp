#ifndef GUARD_SCENE_ACTOR_ALLOCATION_H
#define GUARD_SCENE_ACTOR_ALLOCATION_H

#include "npc/manager/scene_actor_lifecycle.h"
#include "npc/otenko/otenko_actor.h"
#include "npc/story_character/scene_character.h"
#include "npc/story_character/state_actor.h"

/* The manager and the script-specific handlers address the same allocation. */
union SceneActorAllocation {
    struct SceneActor scene;
    struct SceneNpc story;
    struct OtenkoActor otenko;
    struct StateActor state;
};

typedef char SceneActorAllocationSizeCheck[sizeof(union SceneActorAllocation) == 0x384 ? 1 : -1];

#endif
