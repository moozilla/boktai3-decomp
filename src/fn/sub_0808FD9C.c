#include "scene_actor_chase.h"
#ifdef NONMATCHING
// fuzzy: 99%
// Open: one `movs r5, #0` is scheduled before the flags `orrs` in the original.
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

static inline void SetBit(struct SceneActorReactionMotion *m, u32 bit)
{
    u32 v = bit;
    v |= m->flags;
    m->flags = v;
}

void Slime_FollowPlayer(struct SceneActorRegistryResetView *actor)
{
    struct Q *q = actor->context;
    struct P *p;
    u32 zero;
    struct T *dst;
    struct SceneActorReactionMotion *m = (struct SceneActorReactionMotion *)&actor->fieldDC;
    SetBit(m, 8);
    zero = 0;
    dst = &q->pos;
    p = (struct P *)gUnk_02000580[0];
    *dst = p->pos;
    if (q->a >= q->b) {
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = 0x2A;
        actor->target = Slime_UpdateShake;
        actor->field2C8 = zero;
        {
            u32 *field = &actor->flags114;
            u32 mask = ~1;
            *field &= mask;
        }
        actor->word104 = zero;
        Chase_SetState(actor, 3, 0, 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Slime_FollowPlayer);
#endif
