#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.0%
// foundry draft (worker best 85.0%); what differs is mostly register allocation and instruction order
#define ActorAction_FinishRecoveryAnimation sub_0819E25C
struct ActorActionSequence {
    u8 unknown0[0x56];
    s16 height;
    u8 unknown58[0x9A - 0x58];
    u8 active;
    u8 unknown9B[0xAA - 0x9B];
    u8 animation;
    u8 unknownAB[4];
    u8 animationDone;
    u8 entered;
    u8 unknownB1[0xC4 - 0xB1];
    u32 timer;
    u8 unknownC8[0x300 - 0xC8];
    u8 action[0x10];
    u8 unknown310[8];
    u16 cooldown;
    u8 unknown31A[0x383 - 0x31A];
    u8 done;
    u8 unknown384[4];
    u8 savedMode;
};
static inline u8 ConsumeEntry(u8 *p)
{
    if (p[0xB0]) {
        p[0xB0] = 0;
        p[0xAF] = 0;
        return 1;
    }
    return 0;
}
void sub_0819E25C(struct ActorActionSequence *p)
{
    u32 base = (u32)p;
    u32 *timer;
    u8 completed;
    if (ConsumeEntry((u8 *)p)) {
        u32 animation = 0x22;
        p->animation = animation;
    }
    completed = *((u8 *)p + 0xAF);
    timer = &p->timer;
    if (completed) {
        u32 saved = *(u8 *)(base + 0x388);
        u32 zero = 0;
        *((u8 *)p + 0xAC) = zero;
        *((u8 *)p + 0xAD) = zero;
        *((u8 *)p + 0xAE) = saved;
        *timer = zero;
        p->entered = 1;
        *(u8 *)(base + 0x383) = 1;
        *(u16 *)(base + 0x318) = 60;
    }
    (*timer)++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0819E25C);
#endif
