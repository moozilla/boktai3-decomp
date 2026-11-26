#include "scene/collision/scene_body_api.h"
#include "scene/actor/scene_actor_components.h"

u32 SceneBody_SetSprite(struct SceneActorBody *body, void *sprite)
{
    body->sprite = sprite;
    return 1;
}
