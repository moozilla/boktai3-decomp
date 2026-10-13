#include "global.h"

struct E {
    u8 pad0[0x44]; u32 f44; u8 pad48[0x10]; s32 f58; u8 pad5c[4]; s32 f60; u8 f64; u8 pad65[3]; u32 f68;
    u32 f6c;
};

void sub_08219DD8(void *, s32);

u32 sub_0803D8A8(u8 *a, struct E *e)
{
    u32 r = 0;
    u32 z;
    if (*(s8 *)(a + 0x5ec) <= 0 && e->f60 > 0x4f) {
        z = 0;
        e->f60 = z;
        if (e->f58 > 0) {
            e->f68 = 1;
            e->f44 = z;
            e->f64 = 1;
            sub_08219DD8(&e->f6c, 0x100);
        } else {
            e->f68 = 2;
            e->f44 = z;
            e->f64 = 1;
            sub_08219DD8(&e->f6c, 0x100);
        }
        r = 1;
    }
    return r;
}
