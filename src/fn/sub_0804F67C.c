#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0804F5F4(u8 *, u32, u32);
void sub_0821A0C0(u8 *);
void sub_0804F3D8(void);
void sub_0804F418(void);
u8 *sub_0804F67C(u32 a, u32 b)
{
    u8 *r = sub_08219FBC(8, 0x268);
    if (r) {
        sub_0821A04C(r, (void (*)(void))sub_0804F3D8, (void (*)(void))sub_0804F418);
        if (sub_0804F5F4(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
