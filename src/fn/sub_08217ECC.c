#include "global.h"
#include "render.h"

struct Obj {
    u8 pad[0xE];
    u8 fE;
};
u32 Palette_CacheReservedObjectBank(u32, u8 *);

void Effect_SetPalette(struct Obj *o, u32 i)
{
    o->fE = Palette_CacheReservedObjectBank(i, gPaletteBankData + (i << 5));
}
