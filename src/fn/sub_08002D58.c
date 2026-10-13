#include "palette_controller.h"

void Palette_BlendRange(u16 *, const u16 *, const u16 *, u32, s32, s32);

void sub_08002D58(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 *buf = p->blendBuffer;
    u16 *b = t->from;
    u16 *c = t->to;
    Palette_BlendRange(buf, b, c, t->level, 0, 0xd0);
    p->current = buf;
    p->dirty = 1;
}
