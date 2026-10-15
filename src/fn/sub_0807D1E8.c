#include "scene_actor_position_search.h"
/* The signed byte index must address valid storage in the active 16-byte table. */
void SceneActorPosition_ResolveTaggedPosition(struct SceneActorTaggedPosition *target,
                                              const struct SceneActorPositionSource *source,
                                              void *context, s32 index)
{
    struct ContactPosition *position = &target->position;
    u32 tag = sub_0822B694((u16 *)position, (s8)index);
    target->tag = tag;
    if (tag == 0xB546)
        sub_0807CDC4(source, context, position, position);
}
