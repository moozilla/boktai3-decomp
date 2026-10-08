#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081117F0(u8 *);
void sub_0821A0C0(u8 *);
void sub_081113D0(void);
void sub_081113EC(void);

u8 *sub_081119B8(void)
{
    u8 *p;
    p = sub_08219FBC(9, 0x1ae4);
    if (p != 0) {
        sub_0821A04C(p, sub_081113D0, sub_081113EC);
        if (sub_081117F0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
