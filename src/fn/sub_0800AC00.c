#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0800AB60(void *, void *);
void sub_0821A0C0(void *);
void sub_0800A8E4(void);
void sub_0800AB18(void);

void *sub_0800AC00(void *a)
{
    void *p = sub_08219FBC(8, 0x7DC);
    if (p != 0) {
        sub_0821A04C(p, sub_0800A8E4, sub_0800AB18);
        if (sub_0800AB60(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
