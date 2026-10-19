#include "global.h"

#ifdef NONMATCHING
// fuzzy: 95.7%
// foundry draft (worker best 95.7%); what differs is mostly register allocation and instruction order
#define PhaseObject_TickEntryReset sub_080F2AAC
#define PhaseObject_ResetMotion    Ivy_InitBobbing
struct PhaseEntry {
    u32 flags;
    u8 pad4[2];
    u8 phase;
    u8 pad7[0x25];
};
struct PhaseObject {
    u8 pad0[0x8d];
    u8 flags8D;
    u8 pad8E[4];
    u8 mode;
    u8 step;
    u8 pad94[0x14];
    struct PhaseEntry entries[4];
    u8 pad158[0x358 - 0x158];
    u8 pad358[0x4b0 - 0x358];
    s8 resetCount;
    u8 pad4B1[0xb6e - 0x4b1];
    u8 phase;
    u8 padB6F[0xcc8 - 0xb6f];
    void (*update)(struct PhaseObject *);
    u8 padCCC[0xcd8 - 0xccc];
    void (*callback)(struct PhaseObject *);
    u8 padCDC[0xd80 - 0xcdc];
    u32 frame;
};
void Ivy_InitBobbing(struct PhaseObject *);
void ChildGroup_UpdateTimedOscillation(struct PhaseObject *);
void Ivy_WaitForPrimaryActionCompletion(struct PhaseObject *);
void sub_080F2AAC(struct PhaseObject *p)
{
    s32 i;
    if (!(p->frame & 7)) {
        if (++p->resetCount > 7)
            p->resetCount = 7;
    }
    for (i = 0; i < p->resetCount; i++) {
        if (++p->phase == 0x80) {
            for (; i <= 3; i++) {
                p->entries[i].phase = 0x80;
                p->entries[i].flags |= 1;
            }
            p->resetCount = 0;
            Ivy_InitBobbing(p);
            p->mode = 1;
            {
                u32 zero = 0;
                p->step = zero;
            }
            {
                u32 one = 1;
                p->flags8D = one | p->flags8D;
            }
            p->update = ChildGroup_UpdateTimedOscillation;
            p->callback = Ivy_WaitForPrimaryActionCompletion;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F2AAC);
#endif
