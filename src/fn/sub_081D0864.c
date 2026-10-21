// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct Pt081D0864 {
    u16 h0, h2;
};

struct Sub081D0864 {
    u8 pad[9];
    u8 counter;
    s16 offset;
};

struct SceneNpc081D0864 {
    u8 pad0[5];
    u8 b5;
    u8 pad6[8];
    u16 flags;
    u8 pad10[0x20 - 0x10];
    struct Sub081D0864 sub20;
    u8 pad2C[0x6A - 0x2C];
    u16 h6A;
    u8 pad6C[0x130 - 0x6C];
    u16 h130;
    u8 pad132[0x160 - 0x132];
    struct Pt081D0864 *posSrc;
};

struct Ctx081D0864 {
    u8 pad[0x434];
    struct Pt081D0864 *p434;
};

void SceneNpc_DispatchCommands(void *);
int Actor_UpdateMovementAnimationAndEffects(void *, void *);

int sub_081D0864(struct Ctx081D0864 *ctx, struct SceneNpc081D0864 *npc)
{
    struct Sub081D0864 *sub = &npc->sub20;
    u32 m2, t;
    SceneNpc_DispatchCommands(npc);
    m2 = 2;
    if (m2 & npc->flags) {
        struct Pt081D0864 *p = ctx->p434;
        if (p) {
            u32 m = 0x400;
            npc->flags |= m;
            npc->h6A = p->h2;
        }
    }
    Actor_UpdateMovementAnimationAndEffects(ctx, npc);
    {
        u32 m8 = 8;
        if (!(m8 & npc->flags))
            npc->posSrc->h2 += 0xA0;
    }
    {
        u32 m1 = 1;
        if (!(m1 & npc->flags)) {
            register u8 c asm("r1") = sub->counter;
            t = 3;
            if (!(t & c)) {
                t &= npc->h130;
                if (t == 0 || t == 3)
                    sub->offset -= 8;
                else
                    sub->offset += 8;
                if (sub->offset > 0x40)
                    sub->offset = 0x40;
                else if (sub->offset < -0x40)
                    sub->offset = -0x40;
            }
            sub->counter = c + 1;
            asm("" ::"r"(c));
            npc->posSrc->h2 += sub->offset;
        }
    }
    npc->b5 = 0;
    return 0;
}
