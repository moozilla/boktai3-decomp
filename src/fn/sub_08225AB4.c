#include "scene_body_api.h"
#include "scene_actor_components.h"

void SceneBody_AppendToRegistry(void *body);

u32 SceneBody_Init(struct SceneActorBody *body, u32 id, const struct ContactPosition *position,
                   u8 direction, u8 mode, void *owner)
{
    u32 zero;
    body->id = id;
    zero = 0;
    body->flags = zero;
    body->mode = mode;
    body->position = *position;
    body->direction = direction;
    body->motion.x = zero;
    body->motion.y = zero;
    body->motion.z = zero;
    body->motion.pad = 0x10;
    body->movementProbe = 0;
    body->field24 = zero;
    body->sprite = 0;
    body->currentRoute = 0;
    SceneBody_AppendToRegistry(body);
    body->owner = owner;
    return 1;
}
