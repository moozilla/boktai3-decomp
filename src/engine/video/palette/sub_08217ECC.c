#include "global.h"
#include "engine/video/render.h"
#include "engine/video/sprite_drawable_api.h"

u32 Palette_CacheReservedObjectBank(u32, u8 *);

void Effect_SetPalette(struct SpriteDrawable *o, u32 i)
{
    o->palette = Palette_CacheReservedObjectBank(i, gPaletteBankData + (i << 5));
}
