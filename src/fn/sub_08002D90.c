#include "palette_controller.h"

u16 *sub_08002AC0(struct PaletteController *p, u32 set, u32 slot);
void Palette_BlendRange(u16 *, const u16 *, const u16 *, u32, s32, s32);

void sub_08002D90(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 *r;
    u16 *d;
    if (p->flags & 1) {
        r = p->blendBuffer;
    } else if (p->flags & 8) {
        r = p->palettes[p->selected];
    } else {
        r = sub_08002AC0(p, 0, p->selected);
    }
    d = p->effectBuffer;
    Palette_BlendRange(d, r, t->colors, t->brightness, 0, 0xD0);
    p->current = d;
    p->dirty = 1;
}
