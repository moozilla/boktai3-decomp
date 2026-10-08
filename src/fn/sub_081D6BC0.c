#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081D6984(void *, void *);
void sub_0821A0C0(void *);
void sub_081D61C0(void);
void sub_081D68AC(void);
void *sub_081D6BC0(void *a)
{
    void *p = sub_08219FBC(8, 0xf20);
    if (p) {
        sub_0821A04C(p, sub_081D61C0, sub_081D68AC);
        if (sub_081D6984(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
