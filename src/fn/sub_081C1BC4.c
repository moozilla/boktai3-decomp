#include "global.h"

struct E {
    u32 fl;
    u8 pad4[0xC];
    u16 a10;
    u8 pad12[0xC];
    u16 a1E;
    u8 pad20[0xC];
};

struct S {
    u8 f[0x250];
    struct E e[6];
    u8 pad358[0x374 - 0x358];
    u16 a374;
    u8 pad376[0x590 - 0x376];
    u16 mode;
};

void ControlPanel_TickMultiplierTimer(struct S *p)
{
    u16 *a374 = &p->a374;
    u32 copy = *a374;
    s32 countdown = copy;
    s32 i;

    if (*a374 == 0)
        return;

    if (p->mode == 1) {
        s32 newVal = countdown - 1;
        *a374 = newVal;
        if ((s16)newVal == 0) {
            for (i = 0; i <= 5; i++)
                p->e[i].fl |= 1;
        } else {
            for (i = 0; i <= 5; i++)
                p->e[i].a1E += 2;
        }
        return;
    }

    if (*a374 > 3) {
        for (i = 0; i <= 5; i++)
            p->e[i].a1E -= 2;
    } else {
        for (i = 0; i <= 5; i++)
            p->e[i].a1E += 2;
    }
    p->a374 -= 1;
}
