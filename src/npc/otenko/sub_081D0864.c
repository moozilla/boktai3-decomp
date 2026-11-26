#include "npc/otenko/otenko_actor.h"

struct SceneNpcBobContext {
    u8 pad[0x434];
    const struct ContactPosition *targetPosition;
};

void Otenko_DispatchCommands(void *);
int Actor_UpdateMovementAnimationAndEffects(void *, void *);

int sub_081D0864(struct SceneNpcBobContext *ctx, struct OtenkoActor *npc)
{
    struct OtenkoEffectState *effect = &npc->effect;
    u32 m2, t;
    union {
        u32 flags;
        u8 counter;
    } work;
    Otenko_DispatchCommands(npc);
    m2 = 2;
    if (m2 & npc->displayFlags) {
        const struct ContactPosition *p = ctx->targetPosition;
        if (p) {
            u32 m = 0x400;
            npc->displayFlags |= m;
            npc->body.position.y = p->y;
        }
    }
    Actor_UpdateMovementAnimationAndEffects(ctx, npc);
    {
        u32 m8 = 8;
        if (!(m8 & npc->displayFlags))
            ((struct ContactPosition *)npc->display.position)->y += 0xA0;
    }
    {
        work.flags = 1;
        if (!(work.flags & npc->displayFlags)) {
            work.counter = effect->bobCounter;
            t = 3;
            if (!(t & work.counter)) {
                t &= npc->display.resource.sequence.playback.index;
                if (t == 0 || t == 3)
                    effect->bobOffset -= 8;
                else
                    effect->bobOffset += 8;
                if (effect->bobOffset > 0x40)
                    effect->bobOffset = 0x40;
                else if (effect->bobOffset < -0x40)
                    effect->bobOffset = -0x40;
            }
            effect->bobCounter++;
            ((struct ContactPosition *)npc->display.position)->y += effect->bobOffset;
        }
    }
    npc->nextState = 0;
    return 0;
}
