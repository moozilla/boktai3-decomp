#include "global.h"
extern u32 gUnk_020002F8[];
void sub_082195E0(u8 *);
void sub_081DC448(u8 *p)
{
    if (gUnk_020002F8[2] == 1) {
        sub_082195E0(p + 0xa4);
        sub_082195E0(p + 0x1c);
    }
}
