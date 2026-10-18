#include "global.h"

s32 ChallengeMenu_ConsumeStateEntry(u8 *p)
{
    u8 *q = p + 0x2e1;
    s32 r;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}
