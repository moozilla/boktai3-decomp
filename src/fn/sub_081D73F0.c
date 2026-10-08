#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081D7300(void *, void *);
void sub_0821A0C0(void *);
void sub_081D72A4(void);
void sub_081D72FC(void);
void *sub_081D73F0(void *a)
{
    void *p = sub_08219FBC(8, 0x328);
    if (p) {
        sub_0821A04C(p, sub_081D72A4, sub_081D72FC);
        if (sub_081D7300(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
