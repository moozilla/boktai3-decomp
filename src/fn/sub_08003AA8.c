#include "global.h"
#include "color_fade_controller.h"

extern struct ColorFadeController *gUnk_02000020;
s32 sub_08003AA8(u32 id, s32 cnt, u32 *src)
{
    struct ColorFadeController *s = gUnk_02000020;
    s32 i;
    if (s == 0 || s->count > 7)
        return -1;
    s->types[s->count] = id;
    for (i = 0; i < cnt; i++)
        s->args[s->count][i] = *src++;
    s->count++;
    return 0;
}
