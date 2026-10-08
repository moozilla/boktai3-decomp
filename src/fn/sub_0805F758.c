#include "global.h"

struct P { u8 f[0x22]; s16 v; };

s32 sub_0805F758(struct P *p, s32 a, s32 b)
{
    if (b <= 0xb) {
        if ((b & 2) == 0) {
            p->v = a - ((0x14 - b) >> 3);
            return 0;
        }
    } else if (b > 0xf) {
        return 1;
    }
    p->v = a;
    return 0;
}
