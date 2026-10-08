#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0800DC90(void *);
void sub_0821A0C0(void *);
void sub_0800DA5C(void);
void sub_0800DC38(void);

void *sub_0800E08C(void) {
    void *p = sub_08219FBC(8, 0x52C);
    if (p) {
        sub_0821A04C(p, sub_0800DA5C, sub_0800DC38);
        if (sub_0800DC90(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
