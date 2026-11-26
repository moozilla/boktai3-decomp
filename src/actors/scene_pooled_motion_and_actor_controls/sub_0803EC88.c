#include "engine/shared/scene_pool.h"

u32 ScenePool_StateBrake(void *manager, struct ScenePoolActor *actor, struct ScenePoolEntry *entry)
{
    if (entry->stateEntered)
        entry->stateEntered = 0;
    actor->controlFlags |= 0x20;
    ScenePoolActor_SetStateFlags(actor, 0x10);
    entry->stateFrame++;
    return 0;
}
