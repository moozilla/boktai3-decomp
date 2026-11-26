#include "global.h"

// `self`, a copy of p made at entry, keeps its own register in the phase checks
// (spellings.sp_param_alias). With p alone the two phase branches end alike and
// are cross-jumped into one tail.
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
    struct Slot7Motion *self = p;
    if (p->reset)
        BeginPhase(p, 1);
    Callback_InvokeActor(p, gUnk_08613DA8[p->phase]);
    if (((u8)(p->phase - 1)) <= 1) {
        if ((self->timer > 60) || ((self->speed <= 0x1FFF) && (self->timer > 30))) {
            if (self->previousPhase == 4) {
                BeginPhase(self, 3);
                self->previousPhase = 3;
            } else {
                BeginPhase(self, 4);
                self->previousPhase = 4;
            }
        }
    }
}
