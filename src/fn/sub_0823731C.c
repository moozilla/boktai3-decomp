#include "global.h"

extern u8 *gUnk_020004A0;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_082372F8(u8 *);
void sub_0821A0C0(u8 *);
void sub_0823722C(void);
void sub_08237258(void);

u8 *sub_0823731C(void)
{
    u8 *p;
    if (gUnk_020004A0 != 0)
        return gUnk_020004A0;
    p = sub_08219FBC(0xa, 0x19c);
    if (p != 0) {
        sub_0821A04C(p, sub_0823722C, sub_08237258);
        if (sub_082372F8(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
