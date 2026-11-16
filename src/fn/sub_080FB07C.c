#include "global.h"
#include "scene_actor_reset.h"

struct Q {
    u8 filler[0xe48];
    s32 low;
    u8 filler2[0xeb4 - 0xe4c];
    u16 mid;
    u16 pad;
    u16 b[4];
};

static inline void Go(struct ChaseActor *actor, u8 s, u8 p, u8 b)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

u32 Axe_PickAttack(struct ChaseActor *actor)
{
    struct Q *q = actor->base.context;
    if (actor->base.health <= q->low && q->b[3] == 0) {
        Go(actor, 0, 2, 8);
        return 1;
    }
    if (q->b[0] == 0 && actor->base.tracking.distance <= 0xFFFE) {
        s32 a = actor->base.tracking.heading;
        s32 d = a - 0xa0;
        s32 e;
        if (d > 0x80)
            d = a - 0x1a0;
        else if (d < -0x80)
            d = a + 0x60;
        e = d;
        if (e < 0)
            e = -e;
        if (e <= 0x29)
            Go(actor, 0, 2, 0);
        else
            Go(actor, 0, 2, 2);
        return 1;
    }
    if (q->mid >= actor->base.health) {
        if (q->b[1] == 0) {
            Go(actor, 0, 2, 4);
            return 1;
        }
        if (q->b[2] == 0) {
            Go(actor, 0, 2, 6);
            return 1;
        }
    }
    return 0;
}
