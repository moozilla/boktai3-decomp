#include "global.h"

void sub_082156B8(u32);
void sub_0821AD08(u32, u32);

void sub_08109584(u8 *p)
{
    u32 v;

    sub_082156B8(0);
    sub_082156B8(1);
    sub_082156B8(2);
    sub_082156B8(3);
    v = *(u32 *)(p + 0x44);
    if (v != 0) {
        sub_0821AD08(v, 0);
    }
}
