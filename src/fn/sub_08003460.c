#include "global.h"

struct S { u8 f[0x1c]; u8 v; };
extern struct S *gUnk_02000468;

u8 sub_08003460(void)
{
    if (gUnk_02000468 == 0)
        return 0;
    return gUnk_02000468->v;
}
