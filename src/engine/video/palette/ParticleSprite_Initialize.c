#include "global.h"
#include "engine/video/sprite_drawable_api.h"
#include "engine/video/render.h"
#include "effects/sprites/sprite_effect_controller.h"

u32 Palette_CacheReservedObjectBank(u32, void *);
void ParticleSprite_Initialize(struct SpriteDrawable *sprite,
                               const struct SpriteEffectTileResource *resource, u32 flags)
{
    if (!sprite->linked) {
        u32 list;

        sprite->flags = flags;
        sprite->attributes = 0;
        sprite->priority = 2;
        sprite->depthOffset = 0;
        sprite->angle = 0;
        sprite->scaleX = 0x40;
        sprite->scaleY = 0x40;
        Effect_SetOrigin(sprite, 0, 0);
        SpriteDrawable_SelectTileFrame(sprite, resource, 0);
        sprite->palette = Palette_CacheReservedObjectBank(
            resource->paletteBank, gPaletteBankData + (resource->paletteBank << 5));
        if (flags & 0x80)
            list = 1;
        else
            list = 0;
        sprite->previous = NULL;
        sprite->next = NULL;
        Effect_LinkToList(sprite, list);
    }
}
