#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.1%
// Stack-vector initialization and register allocation remain different; pins retain the closest candidate.
// MATCHING_CONCESSION: register-pinning
#define PauseMenu_CreateSelectionWidgets sub_08169504
void sub_0821983C(void *, void *, u32, u32, s32, s32, s32, void *);
struct Position {
    s16 x, y, z, w;
};
void PauseMenu_CreateSelectionWidgets(u8 *p)
{
    s16 a[4], b[4], c[4], d[4];
    s32 i, j;
    u8 *resource, *x, *y, *z;
    a[0] = 8;
    a[1] = 136;
    a[2] = 0;
    a[3] = 0;
    b[0] = 216;
    b[1] = 136;
    b[2] = 0;
    b[3] = 0;
    c[0] = 184;
    c[1] = 136;
    c[2] = 0;
    c[3] = 0;
    resource = p + 0xA4;
    z = p + 0xF70;
    y = p + 0xE28;
    x = p + 0xC88;
    j = 0;
    for (i = 2; i >= 0; i--) {
        sub_0821983C(x, resource, 0x184, 0x10011, 0, 0, 60, &a);
        sub_0821983C(y, resource, 0xF0, 0x10011, 0, 0, 60, &b);
        c[1] = 136 - j;
        sub_0821983C(z, resource, 0xE7, 0x10011, 0, 0, 60, &c);
        z += 0x60;
        j += 24;
        y += 0x60;
        x += 0x60;
    }
    {
        register u8 *res asm("r8") = p + 0x64;
        register u32 flags asm("r9") = 0x10011;
        register s16 *dx asm("r6") = d;
        *dx = 0;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x1090, res, 0x90, flags, 0, 0, 60, &d);
        *dx = 176;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x10F0, res, 0x90, flags, 0, 0, 60, &d);
        *dx = 208;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x1150, res, 0x90, flags, 0, 0, 60, &d);
        *dx = 12;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x11B0, res, 0x90, flags, 0, 0, 60, &d);
        *dx = 188;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x1210, res, 0x90, flags, 0, 0, 60, &d);
        *dx = 220;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x1270, res, 0x90, flags, 0, 0, 60, &d);
        *dx = 0;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x12D0, res, 0x8F, flags, 0, 0, 60, &d);
        *dx = 176;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x1330, res, 0x8F, flags, 0, 0, 60, &d);
        *dx = 208;
        d[1] = 0;
        d[2] = 0;
        d[3] = 0;
        sub_0821983C(p + 0x1390, res, 0x8F, flags, 0, 0, 60, &d);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08169504);
#endif
