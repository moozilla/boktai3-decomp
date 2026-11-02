#include "transient_sprite_pool.h"
#include "render.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
void *SpritePool_CreateSingleton(void);
void Sprite_CopyRenderState(void *, void *, void *, void *);
s32 Sprite_LinkToList(void *, u32);

struct TemporarySpriteSlot *TransientSprite_Spawn(struct DisplaySequenceSprite *source,
                                                  struct TransientSpriteGraphics *graphics,
                                                  u32 lifetime, u32 flags, s32 palette,
                                                  u8 *override)
{
    struct TemporarySpriteSlot *slot;
    struct DisplaySequenceSprite *sprite;
    struct TransientSpriteGraphics *extra;
    u8 *colors;
    struct TemporarySpritePool *pool = (void *)gTransientSpritePool;
    if (pool == 0) {
        pool = SpritePool_CreateSingleton();
        if (pool == 0)
            return 0;
    }
    slot = TransientSpritePool_FindFreeSlot(pool);
    if (slot == 0)
        return 0;
    sprite = &slot->sprite;
    extra = &slot->graphics;
    Sprite_CopyRenderState(sprite, source, extra, graphics);
    sprite->flags |= flags;
    if (palette > 0) {
        extra->paletteIndex = palette;
        colors = override;
        if (colors == 0)
            colors = gPaletteBankData + (palette << 5);
        extra->palette = colors;
    }
    if (sprite->flags & 0x80)
        Sprite_LinkToList(sprite, 1);
    else
        Sprite_LinkToList(sprite, 0);
    {
        register u16 *ip asm("r1") = (u16 *)&lifetime;
        register u16 v asm("r1");
        v = *ip;
        *(u16 *)&slot->lifetime = v;
    }
    slot->active = 1;
    return slot;
}
