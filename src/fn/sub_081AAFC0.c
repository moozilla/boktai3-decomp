
#ifdef NONMATCHING
// fuzzy: 94.4%
// foundry draft (worker best 94.4%); what differs is mostly register allocation and instruction order
#define Vanargand_UpdateAnimationGate sub_081AAFC0
#include "global.h"
struct Vanargand {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[7];
    u8 finished;
    u8 padB0;
    u8 pending;
    u8 animationDone;
    u8 padB3[0x18c - 0xb3];
    u8 animation[0x60];
    u8 drawable[0x5b0 - 0x1ec];
    u16 delay;
    u8 pad5B2[6];
    u16 rate1;
    u16 rate2;
};
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void sub_081AAFC0(struct Vanargand *p, s32 tick)
{
    void *animation = p->animation;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        p->delay = 16;
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (tick == 16)
                SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 15, 2, 4);
            if (tick > 16 && p->animationDone) {
                p->phase = 1;
                p->timer = 0;
                {
                    u16 *rates = &p->rate1;
                    s32 rate = 4;
                    *rates = rate;
                    rates++;
                    *rates = rate;
                }
                return;
            }
            break;
        case 1:
            p->finished = 1;
            return;
    }
    {
        u32 mask = 5;
        p->flags |= mask;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081AAFC0);
#endif
