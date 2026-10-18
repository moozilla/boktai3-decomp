#include "global.h"

struct P {
    u8 f00[0xb8];
    s32 st;
};

s32 PlayerBarrier_IsActive(struct P *p)
{
    if (p != 0 && p->st != 0)
        return 1;
    return 0;
}
