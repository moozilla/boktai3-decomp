#include "global.h"

void sub_08218CB0(void);
u32 Video_GetBackgroundMap(u32);
void sub_08219DD8(u32, u32);

void sub_082156B8(u32 n)
{
    if (n == 0)
        sub_08218CB0();
    sub_08219DD8(Video_GetBackgroundMap(n), 0x800);
}
