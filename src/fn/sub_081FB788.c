#include "global.h"

void *sub_0802140C(s32);
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081FAE0C(void *, void *);
void sub_0821A0C0(void *);
void sub_081FAD14(void);
void sub_081FAD74(void);

void *sub_081FB788(void *a) {
    void *p = sub_0802140C(4);
    if (p != 0) return p;
    p = sub_08219FBC(8, 0xF30);
    if (p != 0) {
        sub_0821A04C(p, sub_081FAD14, sub_081FAD74);
        if (sub_081FAE0C(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
