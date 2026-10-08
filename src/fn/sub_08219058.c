#include "global.h"

void sub_08219058(void)
{
    u32 v = 0x44444444;
    CpuFastSet(&v, (void *)0x0600A000, 0x01000800);
}
