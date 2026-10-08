#include "global.h"
struct S { u16 a, b, c, d, e, f, g; u16 h; u16 i; u16 j; };
extern struct S gUnk_030042A0;
extern u32 gUnk_03004290;
extern u32 gUnk_030042C0[];
extern u32 gUnk_030042C8[];
extern u32 gUnk_030042D0[];
void sub_082152AC(void);
void sub_08215490(u32);
void sub_082142DC(void)
{
    s32 i;
    u32 z;
    u32 *a, *b, *c;
    gUnk_030042A0.a = 0;
    gUnk_030042A0.b = 0;
    gUnk_030042A0.c = 0;
    gUnk_030042A0.g = 1;
    gUnk_030042A0.i = 0;
    gUnk_03004290 = 0;
    z = 0;
    c = gUnk_030042C8; b = gUnk_030042D0; a = gUnk_030042C0;
    for (i = 1; i >= 0; i--) {
        *a++ = z;
        *b++ = z;
        *c++ = z;
    }
    sub_082152AC();
    sub_08215490(1);
}
