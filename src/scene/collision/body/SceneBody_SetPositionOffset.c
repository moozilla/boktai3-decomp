#include "scene/actor/scene_actor_components.h"

u32 SceneBody_SetPositionOffset(struct SceneActorBody *body, const struct ContactPosition *offset)
{
    u32 flag = 4;
    body->flags |= flag;
    body->positionOffset = *offset;
    return 1;
}
