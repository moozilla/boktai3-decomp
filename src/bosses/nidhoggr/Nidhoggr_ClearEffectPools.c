#include "global.h"
struct E {
    u32 a;
    u32 b;
    u8 f[0x6c - 8];
};
struct F {
    u32 a;
    u32 b;
    u8 f[0x188 - 8];
};
struct NidhoggrEffectPool {
    struct E e[24];
    void *resource;
    struct F f[48];
};
void Callback_InvokeActor(void *, u32);
void Nidhoggr_ClearEffectPools(struct NidhoggrEffectPool *pool)
{
    struct E *e;
    struct F *g;
    s32 i;

    e = pool->e;
    for (i = 24; i > 0; i--, e++) {
        e->a = 0;
        if (e->b != 0) {
            Callback_InvokeActor(e, e->b);
            e->b = 0;
        }
    }
    g = pool->f;
    for (i = 48; i > 0; i--, g++) {
        g->a = 0;
        if (g->b != 0) {
            Callback_InvokeActor(g, g->b);
            g->b = 0;
        }
    }
}
