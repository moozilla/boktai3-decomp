#include "scene/collision/scene_body_api.h"
#include "scene/actor/scene_actor_components.h"

void SceneBody_AppendToRegistry(void *body);

u32 SceneBody_Init(struct SceneActorBody *body, u32 id, const struct ContactPosition *position,
                   u32 direction, u8 mode, void *owner)
{
    body->id = id;
    body->flags = 0;
    body->mode = mode;
    body->position = *position;
    body->direction = direction;
    body->motion.x = 0;
    body->motion.y = 0;
    body->motion.z = 0;
    body->motion.pad = 0x10;
    body->movementProbe = 0;
    body->field24 = 0;
    body->sprite = 0;
    body->currentRoute = 0;
    SceneBody_AppendToRegistry(body);
    body->owner = owner;
    return 1;
}
