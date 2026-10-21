// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 83.3% (register-pinned w); open: -1 is materialized via movs/rsbs into ip and loop pointer in r1

struct P {
    u8 pad[0x34C];
    s16 dir;
    s16 arr[4];
};

s32 sub_0823C4B4(struct P *p)
{
    s32 dir = p->dir;
    if (!(dir & 1)) {
        s32 a = (dir + 1) & 7;
        s32 b = (dir + 7) & 7;
        s32 i = 1;
        do {
            register s32 w asm("r2") = p->arr[i - 1];
            if (w != -1) {
                s32 v = w;
                if (v == a || v == b)
                    return v;
                if (w != p->dir)
                    return dir;
            }
            i++;
        } while (i <= 4);
    }
    return dir;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823C4B4);
#endif
