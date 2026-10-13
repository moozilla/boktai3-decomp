#include "global.h"
#include "engine.h"

struct Task08197B7C {
    u8 pad0[0x18];
    u32 target;
    u8 b1c, phase, hold, count;
    u16 id;
    u8 pad22[2];
    u8 *tiles;
    u8 buf[1];
};
void sub_08221208(void *, void *, u32, u32, u32);

void sub_08197B7C(struct Task08197B7C *t)
{
    u8 *src, *dst;
    s32 i;
    if (t->hold == 0) {
        Task_Kill(t);
        return;
    }
    src = t->tiles;
    dst = t->buf;
    if (t->phase <= 8) {
        for (i = 0; i < t->count; i++) {
            sub_08221208(dst, src, 16, t->phase, 3);
            src += 0x20;
            dst += 0x20;
        }
    } else {
        u32 level = 16 - t->phase;
        for (i = 0; i < t->count; i++) {
            sub_08221208(dst, src, 16, level, 3);
            src += 0x20;
            dst += 0x20;
        }
    }
    if (++t->phase > 15)
        t->phase = 0;
    t->hold--;
}
