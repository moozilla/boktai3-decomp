#include "global.h"
extern u8 *gUnk_02000488;
void sub_0822B2F8(u32);
void sub_08057960(u8 *, u32);
void sub_0805B9C8(u8 *p)
{
    u16 *c = (u16 *)(p + 0x4fe);
    if (*c <= 0x1d) {
        *c += 1;
    } else if (p[0xc12] == 0) {
        sub_0822B2F8(0x1ea);
        gUnk_02000488[0x255] = 0xe;
        sub_08057960(p, 9);
    }
}
