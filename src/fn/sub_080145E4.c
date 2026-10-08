#include "global.h"
extern u32 gUnk_02000060;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08014568(void *, void *);
void sub_0821A0C0(void *);
void sub_08014498(void);
void sub_0801450C(void);

void *sub_080145E4(void *a)
{
    void *p;
    if (gUnk_02000060 != 0)
        return (void *)gUnk_02000060;
    p = sub_08219FBC(10, 0xabc);
    if (p != 0) {
        sub_0821A04C(p, sub_08014498, sub_0801450C);
        if (sub_08014568(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
