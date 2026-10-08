#include "global.h"

extern u8 *gUnk_0200059C;

void sub_08160684(u32 v)
{
    if (gUnk_0200059C != 0) {
        if (*(u32 *)(gUnk_0200059C + 0x598) != v)
            *(u32 *)(gUnk_0200059C + 0x584) = 0;
        *(u32 *)(gUnk_0200059C + 0x59C) = v;
    }
}
