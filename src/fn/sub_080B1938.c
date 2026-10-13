#include "global.h"

u8 sub_080B335C(u8 *);
void sub_080B18AC(u8 *);
void sub_080B2858(u8 *, u32);
void sub_080B23A0(u8 *, u32);

void sub_080B1938(u8 *p)
{
    if ((*(u8 **)(p + 0x3d0))[0x6c0]) {
        if (sub_080B335C(p))
            sub_080B18AC(p);
        else
            sub_080B2858(p, 1);
    } else
        sub_080B23A0(p, 1);
}
