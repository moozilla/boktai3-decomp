#include "global.h"
#include "scene_actor_registry.h"


void SceneActorRegistry_SendToCategory(s32, s16 *);

struct VornData {
    u8 unknown0[0x6C1];
    u8 field6C1;
};

void Vorn_SendMessage(struct SceneActorRegistryActor *actor)
{
    struct VornData *ctx = (struct VornData *)actor->context;

    if (ctx->field6C1 != 0) {
        s16 message[2];

        message[0] = 7;
        message[1] = actor->id;
        SceneActorRegistry_SendToCategory(0x19, message);
        ctx->field6C1 = 0;
    }
}
