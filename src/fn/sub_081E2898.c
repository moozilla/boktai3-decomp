#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081E27A8(u8 *);
void sub_0821A0C0(u8 *);
void sub_081E270C(void);
void sub_081E2794(void);

u8 *sub_081E2898(void)
{
    u8 *p;
    p = sub_08219FBC(11, 0x38);
    if (p != 0) {
        sub_0821A04C(p, sub_081E270C, sub_081E2794);
        if (sub_081E27A8(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
