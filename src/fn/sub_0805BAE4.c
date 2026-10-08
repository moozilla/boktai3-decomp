#include "global.h"
void sub_0822B2F8(u32);
void sub_0804A15C(u8 *);
void sub_08057960(u8 *, u32);
void sub_08057978(u8 *, u32);
void sub_0805BAE4(u8 *p)
{
    if (*(u16 *)(p + 0x12e8) == 0) {
        sub_0822B2F8(0x1eb);
        sub_0804A15C(p + 0x510);
        sub_08057960(p, 0xc);
        sub_08057978(p, 5);
    }
}
