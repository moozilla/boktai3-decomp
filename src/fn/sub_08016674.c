#include "global.h"
extern u32 gUnk_02000070;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08016634(void *, u16);
void sub_0821A0C0(void *);
void sub_08016600(void);
void sub_08016620(void);

void *sub_08016674(u32 a)
{
    void *p;
    if (gUnk_02000070 != 0)
        return (void *)gUnk_02000070;
    p = sub_08219FBC(9, 0x34);
    if (p != 0) {
        sub_0821A04C(p, sub_08016600, sub_08016620);
        if (sub_08016634(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
