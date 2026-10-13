#include "global.h"
#include "color_fade_controller.h"

s32 sub_08003860(struct ColorFadeChannel *, s32);
void ColorFade_ApplyCommand(struct ColorFadeController *p, s32 type, s32 *args)
{
    u32 flags = args[0];
    s32 shift = args[1];
    u32 color = args[2] | (args[3] << 5) | (args[4] << 10);
    s32 start = args[5], target = args[6];
    if (flags & 1) {
        struct ColorFadeChannel *c = &p->channels[0];
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
        struct ColorFadeChannel *c = &p->channels[1];
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
