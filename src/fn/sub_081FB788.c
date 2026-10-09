#include "global.h"
void *sub_0802140C(u32);
void *sub_08219FBC(u32, u32);
void sub_081FAD14(void);
void sub_081FAD74(void);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081FAE0C(void *, void *);
void sub_0821A0C0(void *);
void *sub_081FB788(void *arg)
{
    void *p = sub_0802140C(4);
    if (p) return p;
    p = sub_08219FBC(8, 0xf30);
    if (p) {
        sub_0821A04C(p, sub_081FAD14, sub_081FAD74);
        if (sub_081FAE0C(p, arg) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
