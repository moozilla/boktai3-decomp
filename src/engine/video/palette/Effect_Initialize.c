#include "global.h"
#include "engine/video/render.h"
#include "engine/video/sprite_drawable_api.h"
#include "effects/sprites/sprite_effect_controller.h"

u32 Palette_CacheReservedObjectBank(u32, u8 *);

void Effect_Initialize(struct SpriteDrawable *sprite, const struct SpriteEffectTileResource *resource,
                       u32 flags)
{
    if (!sprite->linked) {
        sprite->flags = flags;
        sprite->attributes = 0;
        sprite->priority = 1;
        sprite->depthOffset = 0;
        Effect_SetOrigin(sprite, -8, -8);
        SpriteDrawable_SelectTileFrame(sprite, resource, 0);
        sprite->palette = Palette_CacheReservedObjectBank(
            resource->paletteBank, gPaletteBankData + (resource->paletteBank << 5));
        sprite->previous = NULL;
        sprite->next = NULL;
    }
}
