#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_position_search.h"
#define SceneActor_AdjustTaggedPosition sub_081376B4

u32 sub_081376B4(struct SceneActorRegistryResetView *input, struct ContactPosition *position, s8 index)
{
    u32 tag;
    struct SceneActorRegistryResetView *actor = input;
    tag = sub_0822B694((u16 *)position, index);
    if (tag == 0xB546)
        sub_0807CDC4((const struct SceneActorPositionSource *)&actor->id, actor->routeObject, position, position);
    return tag;
}
