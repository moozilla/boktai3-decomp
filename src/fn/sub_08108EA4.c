#include "global.h"

void sub_08223270(void);

s32 sub_08108EA4(void *p)
{
    if (*(u32 *)((u8 *)p + 0x30) == 0) {
        sub_08223270();
    }
    return *(u32 *)0x020004B0 = 0;
}
