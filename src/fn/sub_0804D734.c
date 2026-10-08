#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0804D6DC(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_0804D620(void);
void sub_0804D6D8(void);

void *sub_0804D734(u32 a, u32 b) {
    void *p = sub_08219FBC(8, 0x1C);
    if (p) {
        sub_0821A04C(p, sub_0804D620, sub_0804D6D8);
        if (sub_0804D6DC(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
