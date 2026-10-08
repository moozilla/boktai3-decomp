#include "global.h"

u32 sub_08219FBC(u32, u32);
void sub_0821A04C(u32, void (*)(void), void (*)(void));
s32 sub_081D15EC(u32, u32);
void sub_0821A0C0(u32);
void sub_081D1404(void);
void sub_081D15C8(void);

u32 sub_081D1688(u32 a)
{
    u32 r = sub_08219FBC(0xb, 0x1fc);
    if (r != 0) {
        sub_0821A04C(r, sub_081D1404, sub_081D15C8);
        if (sub_081D15EC(r, a) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
