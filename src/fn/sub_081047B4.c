#include "global.h"

struct PoolActor {
    u32 flags;
    u8 pad4[0x15C - 4];
    u16 frame;
    u8 started;
};
void sub_08104748(struct PoolActor *);
void CallbackPool_UpdateShake(struct PoolActor *);
void ProximityActor_CheckPlayerRange(struct PoolActor *);

void sub_081047B4(void *unused, struct PoolActor *o)
{
    if (o->started) {
        o->frame = 0;
        o->flags &= ~1;
        o->started = 0;
    }
    sub_08104748(o);
    CallbackPool_UpdateShake(o);
    if (o->frame & 1)
        ProximityActor_CheckPlayerRange(o);
    o->frame++;
}
