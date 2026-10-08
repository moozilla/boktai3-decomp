#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
void sub_0821A0C0(void *);
s32 sub_08212D84(void *, u32);
void sub_0821290C(void);
void sub_08212D64(void);
extern void *gUnk_02000608;
void *sub_08212ED8(u32 a)
{
    void *p = sub_08219FBC(8, 0xbf0);
    if (p) {
        sub_0821A04C(p, sub_0821290C, sub_08212D64);
        if (sub_08212D84(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    gUnk_02000608 = p;
    return p;
}
