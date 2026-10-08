#include "global.h"
extern u32 gUnk_0200009C;
u32 sub_0821ABA8(u32, u32);
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08019130(void *, u32);
void sub_0821A0C0(void *);
void sub_08019044(void);
void sub_08019104(void);
void *sub_08019264(u32 a)
{
    void *p;
    u32 r = sub_0821ABA8(0x61, 0);
    if (gUnk_0200009C != 0) return (void *)gUnk_0200009C;
    if (r != 0) {
        p = sub_08219FBC(0xb, 0x2b0);
    } else {
        p = sub_08219FBC(9, 0x2b0);
    }
    if (p != 0) {
        sub_0821A04C(p, sub_08019044, sub_08019104);
        if (sub_08019130(p, (u16)a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
