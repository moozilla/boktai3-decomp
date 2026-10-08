#include "global.h"

extern u8 *gUnk_0200018C;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0812CDB0(u8 *);
void sub_0821A0C0(u8 *);
void sub_0812CD10(void);
void sub_0812CD70(void);

u8 *sub_0812CDD8(void)
{
    u8 *p;
    if (gUnk_0200018C != 0)
        return gUnk_0200018C;
    p = sub_08219FBC(0xa, 0x63c);
    if (p != 0) {
        sub_0821A04C(p, sub_0812CD10, sub_0812CD70);
        if (sub_0812CDB0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
