#include "global.h"

#define Skeleton_StepPosition sub_080C4768

void sub_080C4768(void *ptr, s32 n)
{
    u16 *p = ptr;
    u32 val;

    switch (n) {
    case 0:
        val = ((p[0] + 0xFF) & 0xFFFFFF00) + 0x80;
        p[0] = val;
        break;
    case 1:
        val = ((p[2] + 0xFF) & 0xFFFFFF00) + 0x80;
        p[2] = val;
        break;
    case 2:
        val = ((p[0] - 0xFF) & 0xFFFFFF00) + 0x80;
        p[0] = val;
        break;
    case 3:
        val = ((p[2] - 0xFF) & 0xFFFFFF00) + 0x80;
        p[2] = val;
        break;
    }
}
