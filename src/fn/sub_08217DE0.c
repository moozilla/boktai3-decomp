#include "global.h"
#include "level_advance_display.h"
#include "render.h"
#include "sprite_effect_controller.h"

u32 Palette_CacheReservedObjectBank(u32, void *);
s32 Effect_LinkToList(struct LevelAdvanceParticle *, u32);
void ParticleSprite_Initialize(struct LevelAdvanceParticle *sprite, void *resource, u32 flags)
{
    const struct SpriteEffectTileResource *tiles = resource;
    u32 zero = sprite->queued;
    if (!zero) {
        sprite->flags = flags;
        sprite->frameState = zero;
        sprite->mode = 2;
        sprite->status = zero;
        sprite->rotation = zero;
        sprite->scaleX = 0x40;
        sprite->scaleY = 0x40;
        Effect_SetOrigin(sprite, 0, 0);
        SpriteDrawable_SelectTileFrame(sprite, resource, 0);
        sprite->paletteSlot = Palette_CacheReservedObjectBank(
            tiles->paletteBank, gPaletteBankData + (tiles->paletteBank << 5));
        {
            u32 queue = (u32)(-(flags & 0x80)) >> 31;
            sprite->previous = (void *)zero;
            sprite->next = (void *)zero;
            Effect_LinkToList(sprite, queue);
        }
    }
}
