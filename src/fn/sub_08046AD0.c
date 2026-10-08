#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08046A10(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_0804694C(void);
void sub_08046A00(void);

void *sub_08046AD0(u32 a, u32 b) {
    void *p = sub_08219FBC(8, 0xC8);
    if (p) {
        sub_0821A04C(p, sub_0804694C, sub_08046A00);
        if (sub_08046A10(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
