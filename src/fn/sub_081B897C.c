#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081B866C(void *, void *);
void sub_0821A0C0(void *);
void sub_081B8518(void);
void sub_081B854C(void);
void *sub_081B897C(void *a)
{
    void *p = sub_08219FBC(8, 0x140);
    if (p) {
        sub_0821A04C(p, sub_081B8518, sub_081B854C);
        if (sub_081B866C(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
