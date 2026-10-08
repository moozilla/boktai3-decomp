#include "global.h"

u32 sub_08219FBC(u32, u32);
void sub_0821A04C(u32, void (*)(void), void (*)(void));
s32 sub_081D4CE0(u32, u32);
void sub_0821A0C0(u32);
void sub_081D4AC0(void);
void sub_081D4CA4(void);

u32 sub_081D4D9C(u32 a)
{
    u32 r = sub_08219FBC(8, 0xa10);
    if (r != 0) {
        sub_0821A04C(r, sub_081D4AC0, sub_081D4CA4);
        if (sub_081D4CE0(r, a) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
