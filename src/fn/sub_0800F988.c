#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0800F924(u8 *);
void sub_0821A0C0(u8 *);
void sub_0800F850(void);
void sub_0800F914(void);

u8 *sub_0800F988(void)
{
    u8 *p;
    p = sub_08219FBC(11, 0x2c);
    if (p != 0) {
        sub_0821A04C(p, sub_0800F850, sub_0800F914);
        if (sub_0800F924(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
