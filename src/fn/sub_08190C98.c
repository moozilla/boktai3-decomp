#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.9%
// foundry draft (worker best 88.9%); what differs is mostly register allocation and instruction order
#define BossParts_SetState sub_08190C98
struct BossParts {
    u8 unknown0[0x71C2];
    u8 state[26];
    u8 progressA[26];
    u8 progressB[26];
    u8 ready[26];
};
s32 sub_08190C98(struct BossParts *boss, s32 index, s32 state)
{
    u8 *base = (u8 *)boss;
    u8 *ready;
    base += 0x7210;
    ready = base + index;
    if (*ready != 0) {
        u8 *stateBase = (u8 *)boss;
        u8 *current;
        stateBase += 0x71C2;
        current = stateBase + index;
        if (*current != state) {
            u32 zero = 0;
            *current = state;
            boss->progressA[index] = zero;
            boss->progressB[index] = zero;
            *ready = zero;
            return 1;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08190C98);
#endif
