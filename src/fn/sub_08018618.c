#include "global.h"
extern u32 gUnk_02000094;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08018600(void *);
void sub_0821A0C0(void *);
void sub_080185B8(void);
void sub_080185F4(void);

void *sub_08018618(void)
{
    void *p;
    if (gUnk_02000094 != 0)
        return (void *)gUnk_02000094;
    p = sub_08219FBC(0xc, 0x39c);
    if (p != 0) {
        sub_0821A04C(p, sub_080185B8, sub_080185F4);
        if (sub_08018600(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
