#include "global.h"

extern void *gUnk_02000114;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_080513B8(void *, void *);
void sub_0821A0C0(void *);
void sub_08051224(void);
void sub_08051390(void);

void *sub_08051574(void *a)
{
    void *p;
    if (gUnk_02000114 != 0)
        return gUnk_02000114;
    p = sub_08219FBC(9, 0x30);
    if (p != 0) {
        sub_0821A04C(p, sub_08051224, sub_08051390);
        if (sub_080513B8(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
