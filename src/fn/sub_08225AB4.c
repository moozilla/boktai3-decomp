#include "scene_body_api.h"
#include "scene_actor_components.h"

void SceneBody_AppendToRegistry(void *body);

u32 SceneBody_Init(struct SceneActorBody *body, u32 id, const struct ContactPosition *position,
                   u32 direction, u8 mode, void *owner)
{
    body->id = id;
    body->flags = 0u;
    body->mode = mode;
    body->position = *position;
    body->direction = direction;
    body->motion.x = 0u;
    body->motion.y = 0u;
    body->motion.z = 0u;
    body->motion.pad = 0x10;
    body->movementProbe = 0;
    body->field24 = 0u;
    body->sprite = 0;
    body->currentRoute = 0;
    SceneBody_AppendToRegistry(body);
    body->owner = owner;
    return 1;
}
