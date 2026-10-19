#include "global.h"

#ifdef NONMATCHING
// fuzzy: 96.5%
// foundry draft (worker best 94.6%); what differs is mostly register allocation and instruction order
#define DirectionalActor_TryBeginState sub_08184FE8
struct DirectionalActor {
    u8 unknown0[0x196];
    u16 flags196;
    u8 unknown198[0x99A];
    u16 flagsB32;
    u8 unknownB34[0xB];
    u8 state, phase, active, counter;
};
s32 sub_08184FE8(struct DirectionalActor *p, u32 state)
{
    if (((volatile struct DirectionalActor *)p)->active) {
        if (p->state != state || p->phase == 0xff)
            goto begin;
    }
    return 0;
begin:
    p->state = state;
    p->phase = 0;
    p->counter = 0;
    p->flags196 |= 4;
    p->flagsB32 &= 0xfffd;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08184FE8);
#endif
