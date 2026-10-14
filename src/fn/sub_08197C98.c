#include "global.h"
#include "engine.h"

struct Task08197C98 {
    u8 pad0[0x18];
    u32 target;
    u8 state, phase, hold, count;
    u16 id;
    u8 pad22[2];
    u8 *tiles;
    u8 buf[1];
};
void sub_08221208(void *, void *, u32, u32, u32);

void sub_08197C98(void *task)
{
    struct Task08197C98 *t = task;
    u8 *src = t->tiles;
    u8 *dst = t->buf;
    s32 i;
    switch (t->state) {
        case 0:
            for (i = 0; i < t->count; i++) {
                sub_08221208(dst, src, 0x7fff, t->phase, 3);
                src += 0x20;
                dst += 0x20;
            }
            if (++t->phase > 7) {
                t->state = 1;
                t->phase = 0;
            }
            break;
        case 1: {
            u32 level = 16 - t->phase;
            for (i = 0; i < t->count; i++) {
                sub_08221208(dst, src, 0x7fff, level, 4);
                src += 0x20;
                dst += 0x20;
            }
            if (++t->phase > 15)
                Task_Kill(task);
            break;
        }
    }
}
