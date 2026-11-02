#include "global.h"

struct SceneNpcPoint {
    u16 x, y;
};

struct SceneNpcBob {
    u8 pad[9];
    u8 counter;
    s16 offset;
};

struct SceneNpcBobActor {
    u8 pad0[5];
    u8 field5;
    u8 pad6[8];
    u16 flags;
    u8 pad10[0x20 - 0x10];
    struct SceneNpcBob bob;
    u8 pad2C[0x6A - 0x2C];
    u16 targetY;
    u8 pad6C[0x130 - 0x6C];
    u16 phase;
    u8 pad132[0x160 - 0x132];
    struct SceneNpcPoint *displayPosition;
};

struct SceneNpcBobContext {
    u8 pad[0x434];
    struct SceneNpcPoint *targetPosition;
};

void SceneNpc_DispatchCommands(void *);
int Actor_UpdateMovementAnimationAndEffects(void *, void *);

int sub_081D0864(struct SceneNpcBobContext *ctx, struct SceneNpcBobActor *npc)
{
    struct SceneNpcBob *bob = &npc->bob;
    u32 m2, t;
    union {
        u32 flags;
        u8 counter;
    } work;
    SceneNpc_DispatchCommands(npc);
    m2 = 2;
    if (m2 & npc->flags) {
        struct SceneNpcPoint *p = ctx->targetPosition;
        if (p) {
            u32 m = 0x400;
            npc->flags |= m;
            npc->targetY = p->y;
        }
    }
    Actor_UpdateMovementAnimationAndEffects(ctx, npc);
    {
        u32 m8 = 8;
        if (!(m8 & npc->flags))
            npc->displayPosition->y += 0xA0;
    }
    {
        work.flags = 1;
        if (!(work.flags & npc->flags)) {
            work.counter = bob->counter;
            t = 3;
            if (!(t & work.counter)) {
                t &= npc->phase;
                if (t == 0 || t == 3)
                    bob->offset -= 8;
                else
                    bob->offset += 8;
                if (bob->offset > 0x40)
                    bob->offset = 0x40;
                else if (bob->offset < -0x40)
                    bob->offset = -0x40;
            }
            bob->counter++;
            npc->displayPosition->y += bob->offset;
        }
    }
    npc->field5 = 0;
    return 0;
}
