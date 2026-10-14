#include "global.h"

extern const u8 gUnk_0824E20C[];
s32 sub_080B0F4C(u8 *);
void sub_080B3110(u8 *, u32, const u8 *, u32);

void sub_080ACD5C(u32 a, u32 b, u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3d0);
    switch (sub_080B0F4C(q)) {
        case 0:
            *(u16 *)(q + 0x682) = 1;
        case 1:
            sub_080B3110(p, 1, gUnk_0824E20C, 0x4a);
            break;
    }
}
