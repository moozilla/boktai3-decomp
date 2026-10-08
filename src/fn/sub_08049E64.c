#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
void sub_080492A4(u8 *);
void sub_08072C40(u8 *);
void sub_08049E64(u32 a)
{
    u8 *p = gUnk_02000488;
    if (p) {
        u8 *q;
        if (a) sub_08049168(p, 7);
        else sub_08049168(p, 6);
        q = p + 0x254;
        if (*q) {
            sub_08072C40(p + 0x244);
            *q = 0;
        }
        *(u16 *)(p + 0x380) = 0;
        sub_080492A4(p);
    }
}
