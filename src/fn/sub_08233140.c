#include "transient_sprite_pool.h"
#include "render.h"
#include "ram.h"
void *SpritePool_CreateSingleton(void);
void Sprite_CopyRenderState(void *, void *, void *, void *);
s32 Sprite_LinkToList(void *, u32);

static inline u8 *TrailPalette(s32 bank, u8 *override)
{
    if (override)
        return override;
    return gPaletteBankData + (bank << 5);
}

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
        colors = TrailPalette(palette, override);
        extra->palette = colors;
    }
    if (sprite->flags & 0x80)
        Sprite_LinkToList(sprite, 1);
    else
        Sprite_LinkToList(sprite, 0);
    slot->lifetime = lifetime;
    slot->active = 1;
    return slot;
}
