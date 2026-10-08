#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0800C654(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_0800C520(void);
void sub_0800C630(void);

void *sub_0800C7AC(u32 a, u32 b) {
    void *p = sub_08219FBC(8, 0x118);
    if (p) {
        sub_0821A04C(p, sub_0800C520, sub_0800C630);
        if (sub_0800C654(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
