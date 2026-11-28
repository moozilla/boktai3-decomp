#include "npc/otenko/otenko_actor.h"

s32 Otenko_ApplyDeceleratingBlink(struct OtenkoActor *, u32, u32);

static inline void orr(u16 *a, u32 m)
{
    *a = m | *a;
}

void Otenko_UpdateBlinkOut(void *context, struct OtenkoActor *npc, u32 tick)
{
    if (Otenko_ApplyDeceleratingBlink(npc, tick, 0x10)) {
        orr(&npc->displayFlags, 0x100);
        npc->done = 1;
    }
}
