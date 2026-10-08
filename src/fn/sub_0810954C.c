#include "global.h"

u32 sub_08033690(u32, u32, u32, u32);
void sub_0803386C(u32, u32);
void sub_08033924(u32, u32);
void sub_080337FC(u32);

void sub_0810954C(u8 *p)
{
    u32 h;

    h = sub_08033690(1, 6, 0x1C, 6);
    *(u32 *)(p + 0x30) = h;
    sub_0803386C(h, *(u32 *)(p + 0x24));
    sub_08033924(*(u32 *)(p + 0x30), 0);
    sub_080337FC(*(u32 *)(p + 0x30));
}
