#include "global.h"

struct Obj {
    u8 pad[0xE];
    u8 fE;
};
extern u8 *gUnk_030042E4;
u32 Palette_CacheReservedObjectBank(u32, u8 *);

void Effect_SetPalette(struct Obj *o, u32 i)
{
    o->fE = Palette_CacheReservedObjectBank(i, gUnk_030042E4 + (i << 5));
}
