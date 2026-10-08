#include "global.h"
void sub_0821AD08(u32, u32);
void sub_08116CC8(u8 *s)
{
    u8 *p = s + 0x868;
    u16 *h;
    u16 m;
    if (*p != 0) {
        *p = 0;
        sub_0821AD08(*(u32 *)(s + 0x904), 0);
    }
    m = 1;
    h = (u16 *)(s + 0x8f4);
    *h = *h | m;
    (*(u32 *)(s + 0x864))++;
}
