#include "global.h"
#define BossParts_SetState Nidhoggr_ChangeCompletedPartState
struct BossParts {
    u8 unknown0[0x71C2];
    u8 state[26];
    u8 progressA[26];
    u8 progressB[26];
    u8 ready[26];
};
s32 Nidhoggr_ChangeCompletedPartState(struct BossParts *boss, s32 index, s32 state)
{
    u8 *base = (u8 *)boss;
    u8 *ready;
    u8 *stateBase;
    u8 *current;
    u32 zero;
    base += 0x7210;
    ready = base + index;
    if (*ready == 0)
        return 0;
    stateBase = (u8 *)boss;
    stateBase += 0x71C2;
    current = stateBase + index;
    if (*current == state)
        return 0;
    zero = 0;
    *current = state;
    boss->progressA[index] = zero;
    boss->progressB[index] = zero;
    *ready = zero;
    return 1;
}
