#include "global.h"
extern const s32 gUnk_086108DC[];
struct S {
    u8 pad[0x1f8];
    u8 counters[3];
};

void sub_08138724(struct S *p)
{
    s32 i;
    for (i = 0; i <= 2; i++) {
        if (p->counters[i] >= gUnk_086108DC[i])
            p->counters[i] = 0;
    }
}
