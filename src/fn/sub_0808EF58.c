#include "scene_actor_chase.h"
void Slime_UpdateLaunch(void);

struct Q {
    u8 filler[0x3d4];
    u16 v;
};
struct AnimState {
    u8 filler[8];
    u16 a;
    u8 filler2[4];
    u16 b;
};

void Slime_UpdateShake(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    struct Q *q = actor->context;
    struct AnimState *st = (struct AnimState *)((u8 *)actor->drawable + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        q->v = 0;
        actor->field2C2 = 0x40;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, 0x40);
        SceneActorAnimation_SelectProfileEntry(actor, 1);
        {
            u32 bit = 0x100;
            actor->flags2D4 = bit | actor->flags2D4;
        }
    }
    st->a = 0;
    st->b = 5;
    if (Mod(elapsed, 4) == 0) {
        u32 mask = 0x20;
        u16 *flags = &actor->flags2D6;
        mask |= *flags;
        *flags = mask;
    }
    if (elapsed > 30)
        Chase_SetCtl(actor, Slime_UpdateLaunch, 0x2B);
}
