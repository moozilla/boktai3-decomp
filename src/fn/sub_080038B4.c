#include "global.h"
struct Channel {
    u32 mode, color, start, level, target;
    u16 time, shift;
};
struct Controller {
    u8 task[0x18];
    struct Channel channels[2];
};
s32 sub_08003860(struct Channel *, s32);
void ColorFade_ApplyCommand(struct Controller *p, s32 type, s32 *args)
{
    u32 flags = args[0];
    s32 shift = args[1];
    u32 color = args[2] | (args[3] << 5) | (args[4] << 10);
    s32 start = args[5], target = args[6];
    if (flags & 1) {
        struct Channel *c = &p->channels[0];
        if (sub_08003860(c, type) >= 0) {
            if (type == 5) {
                c->time = 1;
                c->shift = 0;
            } else {
                c->time = 0;
                c->shift = shift;
            }
            c->color = color;
            c->start = start;
        }
    }
    if (flags & 2) {
        struct Channel *c = &p->channels[1];
        if (sub_08003860(c, type) >= 0) {
            if (type == 5) {
                c->time = 1;
                c->shift = 0;
            } else {
                c->time = 0;
                c->shift = shift;
            }
            c->color = color;
            c->target = target;
        }
    }
}
