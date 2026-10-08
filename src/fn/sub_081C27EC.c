#include "global.h"
extern u32 gUnk_02000260;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C2570(void *);
void sub_0821A0C0(void *);
u32 sub_081C2294(void *);
void sub_081C22BC(void *);
void *sub_081C27EC(void)
{
    void *p;
    if (gUnk_02000260 != 0) return (void *)gUnk_02000260;
    p = sub_08219FBC(8, 0x5bc);
    if (p != 0) {
        sub_0821A04C(p, (void (*)(void *))sub_081C2294, sub_081C22BC);
        if (sub_081C2570(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
