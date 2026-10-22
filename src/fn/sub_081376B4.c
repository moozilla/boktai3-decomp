#include "scene_actor_position_search.h"

// confidence=low: resolves a tagged stimulus position and, for tag 0xB546, snaps it to a reachable neighbour.
u32 sub_081376B4(u8 *actor, u16 *position, s32 index)
{
    u32 tag = SceneStimulus_GetTaggedPosition(position, (s8)index);
    if (tag == 0xB546)
        SceneActorAttachment_FindReachableNeighbor(
            (const struct SceneActorPositionSource *)(actor + 0x50), *(void **)(actor + 0x5d0),
            (struct ContactPosition *)position, (struct ContactPosition *)position);
    return tag;
}
