#include "scene_actor_registry.h"
u32 SceneActorAttachment_TestPositionWindow(struct SceneActorRegistryActor *actor,
                                            const struct ContactPosition *position, u32 xWindow,
                                            u32 zWindow)
{
    s16 x = (u16)position->x - (u16)actor->position.x;
    s16 z = (u16)position->z - (u16)actor->position.z;
    s32 dx = x;
    s32 dz;
    if (dx < 0)
        dx = -dx;
    if ((u32)dx > xWindow)
        return 0;
    dz = z;
    if (dz < 0)
        dz = -dz;
    if ((u32)dz > zWindow)
        return 0;
    return 1;
}
