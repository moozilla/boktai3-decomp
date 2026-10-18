#include "global.h"
#include "scene_actor_registry.h"
#include "animated_hazard.h"

#define Vorn_TickSound sub_080B3404

void sub_0822B7B4(s16 a, s16 b, s16 c, s16 d, u8 e, u16 f);

struct VornData {
    u8 unknown0[0x6EC];
    u16 field6EC;
};

void Vorn_TickSound(struct SceneActorRegistryActor *actor)
{
    struct VornData *ctx = (struct VornData *)actor->context;

    if (ctx->field6EC == 0) {
        struct SceneVec *vec;

        ctx->field6EC = 60;
        Sound_PlaySong(0x1CF);
        vec = sub_0815F53C(0);
        if (vec != 0) {
            sub_0822B7B4(vec->x, vec->z, 0, 0, 0x80, (u16)actor->id);
        }
    } else {
        ctx->field6EC--;
    }
}
