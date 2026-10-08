#include "global.h"

extern u8 *gUnk_0200059C;

void sub_0816275C(u32 v)
{
    if (gUnk_0200059C != 0) {
        if (*(u32 *)(gUnk_0200059C + 0x5A0) == 0)
            *(u32 *)(gUnk_0200059C + 0x24) = v;
    }
}
