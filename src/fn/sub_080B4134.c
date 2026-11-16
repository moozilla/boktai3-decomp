#include "global.h"
#include "vorn.h"
#include "scene_actor_registry.h"

void SceneActorRegistry_SendToCategory(s32, s16 *);

void Vorn_SendMessage(struct ChaseActor *actor)
{
    struct Vorn *ctx = (struct Vorn *)actor->base.context;

    if (ctx->field6C1 != 0) {
        s16 message[2];

        message[0] = 7;
        message[1] = actor->base.body.id;
        SceneActorRegistry_SendToCategory(0x19, message);
        ctx->field6C1 = 0;
    }
}
