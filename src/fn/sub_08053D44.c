#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08053AEC(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_08053AB0(void);
void sub_08053AC4(void);

void *sub_08053D44(u32 a, u32 b) {
    void *p = sub_08219FBC(9, 0x118);
    if (p) {
        sub_0821A04C(p, sub_08053AB0, sub_08053AC4);
        if (sub_08053AEC(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
