#include "global.h"
extern u32 gUnk_02000034;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_080090D4(void *);
void sub_0821A0C0(void *);
void sub_08008F8C(void);
void sub_08009088(void);

void *sub_08009180(void)
{
    void *p;
    if (gUnk_02000034 != 0)
        return (void *)gUnk_02000034;
    p = sub_08219FBC(10, 0x80);
    if (p != 0) {
        sub_0821A04C(p, sub_08008F8C, sub_08009088);
        if (sub_080090D4(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
