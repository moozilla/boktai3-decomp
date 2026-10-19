#include "global.h"

#define ControlPanel_ApplyMultiplierMode sub_081C1B20

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

void Sound_PlaySong(u32 song); // include/engine.h

void sub_081C1B20(struct S *p)
{
    s32 mode = p->mode;

    if (mode == 1) {
        u16 v = 0x30;
        s32 i = 5;
        do {
            p->e[i].a1E = v;
        } while (--i >= 0);
        p->a374 = 4;
        return;
    }
    if (mode == 2) {
        struct E *q = &p->e[1];
        q->a10 = 4;
    } else if (mode == 5) {
        struct E *q = &p->e[1];
        q->a10 = mode;
    } else if (mode == 10) {
        struct E *q = &p->e[1];
        q->a10 = 6;
    }

    {
        s32 i = 0;
        do {
            p->e[i].fl &= ~1;
            p->e[i].a1E = 0x38;
        } while (++i <= 5);
    }
    p->a374 = 10;
    Sound_PlaySong(0x28B);
}
