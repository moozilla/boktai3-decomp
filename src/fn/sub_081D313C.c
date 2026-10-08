#include "global.h"

u32 sub_08219FBC(u32, u32);
void sub_0821A04C(u32, void (*)(void), void (*)(void));
s32 sub_081D3094(u32);
void sub_0821A0C0(u32);
void sub_081D305C(void);
void sub_081D3078(void);

u32 sub_081D313C(void)
{
    u32 r = sub_08219FBC(8, 0x3a8);
    if (r != 0) {
        sub_0821A04C(r, sub_081D305C, sub_081D3078);
        if (sub_081D3094(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
