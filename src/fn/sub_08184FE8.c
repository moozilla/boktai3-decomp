// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct DirectionalActor {
    u8 unknown0[0x196];
    u16 flags196;
    u8 unknown198[0x99A];
    u16 flagsB32;
    u8 unknownB34[0xB];
    u8 state, phase, active, counter;
};
s32 DirectionalActor_TryBeginState(struct DirectionalActor *p, u32 state)
{
    if (((volatile struct DirectionalActor *)p)->active) {
        asm("" ::: "r1");
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
