#include "global.h"

extern void *gUnk_02000038;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08009AA0(void *, void *);
void sub_0821A0C0(void *);
void sub_0800997C(void);
void sub_08009A68(void);

void *sub_08009B30(void *a)
{
    void *p;
    if (gUnk_02000038 != 0)
        return gUnk_02000038;
    p = sub_08219FBC(8, 0x30);
    if (p != 0) {
        sub_0821A04C(p, sub_0800997C, sub_08009A68);
        if (sub_08009AA0(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
