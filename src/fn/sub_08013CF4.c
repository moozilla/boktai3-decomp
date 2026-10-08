#include "global.h"
extern u32 gUnk_02000470;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08013CE4(void *);
void sub_0821A0C0(void *);
void sub_08013CA4(void);
void sub_08013CA8(void);

void *sub_08013CF4(void)
{
    void *p;
    if (gUnk_02000470 != 0)
        return (void *)gUnk_02000470;
    p = sub_08219FBC(9, 0x1c);
    if (p != 0) {
        sub_0821A04C(p, sub_08013CA4, sub_08013CA8);
        if (sub_08013CE4(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
