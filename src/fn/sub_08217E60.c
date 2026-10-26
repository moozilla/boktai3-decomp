#include "global.h"
#include "render.h"

struct Pal {
    u8 pad[6];
    u16 f6;
};
struct Obj {
    u32 f0;
    u8 f4;
    u8 pad5[9];
    u8 fE, fF, f10;
    u8 pad11[3];
    u32 f14;
    u8 pad18[8];
    u32 f20, f24;
};
void Effect_SetOrigin(struct Obj *, s32, s32);
void SpriteDrawable_SelectTileFrame(struct Obj *, struct Pal *, u32);
u32 Palette_CacheReservedObjectBank(u32, u8 *);

void Effect_Initialize(struct Obj *o, struct Pal *p, u32 c)
{
    if (o->f4 == 0) {
        o->f0 = c;
        o->f14 = 0;
        o->fF = 1;
        o->f10 = 0;
        Effect_SetOrigin(o, -8, -8);
        SpriteDrawable_SelectTileFrame(o, p, 0);
        o->fE = Palette_CacheReservedObjectBank(p->f6, gPaletteBankData + (p->f6 << 5));
        o->f20 = 0;
        o->f24 = 0;
    }
}
