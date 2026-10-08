#include "global.h"
struct E { u8 p[0x344]; };
struct A { u8 p0[0x1c]; struct E e[3]; };
extern u32 gUnk_020005F8;
void sub_0821FE6C(void *);
void sub_08013B74(void *);
void sub_08160624(u32);
void sub_0821A0C0(u32);
void sub_082195E0(void *);
u32 sub_08201024(struct A *a)
{
    u8 *e = (u8 *)a + 0x1c;
    s32 i;
    for (i = 2; i >= 0; i--, e += 0x344) {
        u32 *q;
        sub_0821FE6C(e + 0x80);
        sub_08013B74(e + 0x298);
        q = (u32 *)(e + 0x33c);
        if (*q != 0) {
            sub_08160624(*q);
            sub_0821A0C0(*q);
        }
        sub_082195E0(e + 0x1b8);
    }
    return gUnk_020005F8 = 0;
}
