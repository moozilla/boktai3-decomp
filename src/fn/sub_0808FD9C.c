#include "scene_actor_chase.h"
void Slime_UpdateShake(void);

struct T {
    u32 w[2];
};
struct Q {
    u8 filler[0x3d4];
    u16 a;
    u16 b;
    u8 filler2[0x3ec - 0x3d8];
    struct T pos;
};
struct P {
    u8 filler[0x30];
    struct T pos;
};

void Slime_FollowPlayer(struct SceneActorRegistryResetView *actor)
{
    int new_var;
    int new_var2;
    struct Q *q = actor->context;
    struct P *p;
    u32 zero;
    struct T *dst;
    struct SceneActorReactionMotion *m = (struct SceneActorReactionMotion *)&actor->tracking.target;
    {
        u32 v = 8;
        u32 f = m->flags;
        zero = 0;
        v |= f;
        m->flags = v;
    }
    dst = &q->pos;
    p = (struct P *)gPlayers[0];
    *dst = p->pos;
    new_var = 0;
    if (q->a >= q->b) {
        void (*fn)(void) = Slime_UpdateShake;
        u32 st = 0x2A;
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = st;
        actor->target = fn;
        new_var2 = -0x2C;
        actor->field2C8 = zero;
        {
            u32 *field = &actor->flags114;
            u32 mask = st + new_var2;
            *field &= mask;
        }
        actor->word104 = zero;
        Chase_SetState(actor, 3, new_var, new_var);
    }
}
