#include "global.h"
extern u32 gUnk_020000B0;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08019AA8(void *);
void sub_0821A0C0(void *);
void sub_08019A14(void);
void sub_08019A78(void);

void *sub_08019AE4(void)
{
    void *p = (void *)gUnk_020000B0;
    if (p == 0) {
        p = sub_08219FBC(10, 0xb1c);
        if (p != 0) {
            sub_0821A04C(p, sub_08019A14, sub_08019A78);
            if (sub_08019AA8(p) < 0) {
                sub_0821A0C0(p);
                return 0;
            }
        }
    }
    return p;
}
