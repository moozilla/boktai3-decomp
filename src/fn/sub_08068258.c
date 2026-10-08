#include "global.h"

void sub_08068230(void *);

s32 sub_08068258(u8 *p, s32 v)
{
    *(s32 *)(p + 0x724) = v;
    sub_08068230(p);
    return 0;
}
