#include "global.h"
#include "render.h"

struct GlyphEntry {
    u16 a, b, c, d, e, f, g, h;
    u32 off;
};
struct GlyphRes {
    u16 palette;
    u8 pad[0xA];
    struct GlyphEntry *entries;
    u32 pad10;
    u32 base;
    u32 pad18;
    u32 extra;
};
struct Widget {
    u16 w0;
    u8 pad02[0x26];
    u16 w28, w2a, w2c, w2e, w30, w32;
    u8 b34, b35;
    u16 w36, w38, w3a;
    u8 pad3c[0xC];
    u8 *palette;
    u32 data;
    u32 extra;
};

s32 Anim_SelectWidgetGlyph(struct Widget *w, struct GlyphRes *res, u16 index)
{
    struct GlyphEntry *e = res->entries + index;
    u32 zero;
    if (e == 0)
        return -1;
    w->w0 = e->a;
    zero = 0;
    w->w28 = e->c;
    w->w2a = e->d;
    w->w2c = e->e;
    w->w2e = e->f;
    w->w30 = e->g;
    w->w32 = e->h;
    w->b34 = 0x40;
    w->b35 = 0x40;
    w->w36 = zero;
    if (w->palette == 0) {
        w->w3a = res->palette;
        w->palette = gPaletteBankData + (w->w3a << 5);
    }
    w->w38 = e->b;
    w->data = res->base + e->off;
    w->extra = res->extra;
    return 0;
}
