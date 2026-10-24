#include "global.h"

struct Slot7Motion {
    u8 pad0[0xAE];
    u8 phase, reset, entered;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
    u8 padC8[0x2B4 - 0xC8];
    s32 speed;
    u8 pad2B8[0x5B4 - 0x2B8];
    s32 previousPhase;
};
extern void *gUnk_08613DA8[];
s32 Callback_InvokeActor(void *, void *);
static inline void BeginPhase(struct Slot7Motion *p, s32 phase)
{
    p->phase = phase;
    p->timer = 0;
    p->entered = 1;
}
void Slot7_UpdateMotionPhase(struct Slot7Motion *p)
{
    if (p->reset)
        BeginPhase(p, 1);
    Callback_InvokeActor(p, gUnk_08613DA8[p->phase]);
    if ((u8)(p->phase - 1) <= 1) {
        if (p->timer > 60 || (p->speed <= 0x1FFF && p->timer > 30)) {
            if (p->previousPhase == 4) {
                BeginPhase(p, 3);
                p->previousPhase = 3;
            } else {
                BeginPhase(p, 4);
                p->previousPhase = 4;
            }
        }
    }
}
