#include "global.h"
extern u8 *gUnk_02000488;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0804D500(u8 *, u32, u32);
void sub_0821A0C0(u8 *);
void sub_0804D378(void);
void sub_0804D4B0(void);
u8 *sub_0804D5CC(u32 a, u32 b)
{
    u8 *r;
    if (gUnk_02000488 != 0) return gUnk_02000488;
    r = sub_08219FBC(9, 0x484);
    if (r) {
        sub_0821A04C(r, sub_0804D378, sub_0804D4B0);
        if (sub_0804D500(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
