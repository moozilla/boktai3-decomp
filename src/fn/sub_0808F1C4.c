#include "scene_actor_chase.h"
void SceneActorControl_OnIdle(void);

struct D {
    u32 flags;
    u8 filler[4];
    u8 a;
    u8 b;
};

#define Slime_UpdateStill sub_0808F1C4

void Slime_UpdateStill(struct SceneActorRegistryResetView *actor)
{
    struct D *d = actor->drawable;
    if (SceneActorReaction_TakePrepared(actor)) {
        d->a = 0x40;
        d->b = 0x40;
        d->flags &= ~2;
        {
            u16 *f = &actor->flags2D4;
            u32 mask = 0xFFFFFEFF;
            *f = mask & *f;
        }
    }
    if (actor->field2B3) {
        actor->field2C2 = 0x40;
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
        Chase_SetState(actor, 0, 0, 2);
    }
}
