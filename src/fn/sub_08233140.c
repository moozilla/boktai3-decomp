#include "global.h"
#include "render.h"
#include "ram.h"
/* Tentative: allocate a transient sprite from the pool, copy render state into it and link it into the draw list. */
// MATCHING_CONCESSION: register-pinning
u8 *SpritePool_CreateSingleton(void);
u8 *TransientSpritePool_FindFreeSlot(u8 *);
void Sprite_CopyRenderState(void *, void *, void *, void *);
void Sprite_LinkToList(void *, u32);

u8 *TransientSprite_Spawn(void *a, void *b, u32 id, u32 flags, s32 palette, u8 *override)
{
    u8 *slot;
    u8 *sprite;
    u8 *extra;
    u8 *q;
    u8 *pool = gTransientSpritePool;
    if (pool == 0) {
        pool = SpritePool_CreateSingleton();
        if (pool == 0)
            return 0;
    }
    slot = TransientSpritePool_FindFreeSlot(pool);
    if (slot == 0)
        return 0;
    sprite = slot + 4;
    extra = slot + 0x30;
    Sprite_CopyRenderState(sprite, a, extra, b);
    *(u32 *)sprite |= flags;
    if (palette > 0) {
        *(u16 *)(extra + 6) = palette;
        q = override;
        if (q == 0)
            q = gPaletteBankData + (palette << 5);
        *(u8 **)(extra + 0xC) = q;
    }
    if (*(u32 *)sprite & 0x80)
        Sprite_LinkToList(sprite, 1);
    else
        Sprite_LinkToList(sprite, 0);
    {
        register u16 *ip asm("r1") = (u16 *)&id;
        register u16 v asm("r1");
        v = *ip;
        *(u16 *)(slot + 2) = v;
    }
    *slot = 1;
    return slot;
}
