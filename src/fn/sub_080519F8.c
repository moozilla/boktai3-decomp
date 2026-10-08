#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08051858(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_080516FC(void);
void sub_0805173C(void);

void *sub_080519F8(u32 a, u32 b) {
    void *p = sub_08219FBC(8, 0xD0);
    if (p) {
        sub_0821A04C(p, sub_080516FC, sub_0805173C);
        if (sub_08051858(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
