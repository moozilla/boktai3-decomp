#include "engine/shared/global.h"
#include "engine/shared/sprite_drawable_api.h"
#include "engine/shared/render.h"
#include "engine/shared/sprite_effect_controller.h"

u32 Palette_CacheReservedObjectBank(u32, void *);
void ParticleSprite_Initialize(struct SpriteDrawable *sprite,
                               const struct SpriteEffectTileResource *resource, u32 flags)
{
    const struct SpriteEffectTileResource *tiles = resource;
    u32 zero = sprite->linked;
    if (!zero) {
        sprite->flags = flags;
        sprite->attributes = zero;
        sprite->priority = 2;
        sprite->depthOffset = zero;
        sprite->angle = zero;
        sprite->scaleX = 0x40;
        sprite->scaleY = 0x40;
        Effect_SetOrigin(sprite, 0, 0);
        SpriteDrawable_SelectTileFrame(sprite, resource, 0);
        sprite->palette = Palette_CacheReservedObjectBank(
            tiles->paletteBank, gPaletteBankData + (tiles->paletteBank << 5));
        {
            u32 queue = (u32)(-(flags & 0x80)) >> 31;
            sprite->previous = (void *)zero;
            sprite->next = (void *)zero;
            Effect_LinkToList(sprite, queue);
        }
    }
}
