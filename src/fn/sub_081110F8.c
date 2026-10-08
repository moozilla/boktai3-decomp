#include "global.h"
extern u8 *gUnk_020004B8;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08110FD4(u8 *);
void sub_0821A0C0(u8 *);
void sub_08110E14(void);
void sub_08110E38(void);
u8 *sub_081110F8(void)
{
    u8 *p;
    if (gUnk_020004B8 != 0)
        return gUnk_020004B8;
    p = sub_08219FBC(2, 0x60);
    if (p != 0) {
        sub_0821A04C(p, sub_08110E14, sub_08110E38);
        if (sub_08110FD4(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
