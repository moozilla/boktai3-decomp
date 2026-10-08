#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08057714(u8 *);
void sub_0821A0C0(u8 *);
void sub_080576C8(void);
void sub_08057704(void);

u8 *sub_08057924(void)
{
    u8 *p;
    p = sub_08219FBC(9, 0x84);
    if (p != 0) {
        sub_0821A04C(p, sub_080576C8, sub_08057704);
        if (sub_08057714(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
