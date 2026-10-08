#include "global.h"

extern void *gUnk_02000050;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08012684(void *);
void sub_0821A0C0(void *);
void sub_08012630(void);
void sub_08012658(void);

void *sub_080126DC(void)
{
    void *p;
    if (gUnk_02000050 != 0)
        return gUnk_02000050;
    p = sub_08219FBC(8, 0x538);
    if (p != 0) {
        sub_0821A04C(p, sub_08012630, sub_08012658);
        if (sub_08012684(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
