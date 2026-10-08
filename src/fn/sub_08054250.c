#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08054028(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_08053FF8(void);
void sub_0805400C(void);

void *sub_08054250(u32 a, u32 b) {
    void *p = sub_08219FBC(9, 0xCC);
    if (p) {
        sub_0821A04C(p, sub_08053FF8, sub_0805400C);
        if (sub_08054028(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
