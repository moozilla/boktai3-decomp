#include "global.h"
#include "player/effects/recoil_puff_sprite.h"

// confidence=low: initializes eight 0x30-byte sprite entries at 0x18 with animation 0x1C1A, frame 12,
// origin (-8, -8), shaped like SpriteGroup_InitFourSprites.
void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

void sub_08237280(struct RecoilPuffPool *p)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        struct RecoilPuffSprite *sprite = &p->entries[i];
        sprite->active = 0;
        sprite->phase = 0;
        sprite->timer = 0;
        sprite->resource = AnimData_FindEntry(0x1C1A);
        ParticleSprite_Initialize(sprite, sprite->resource, 0);
        Effect_SetOrigin(sprite, -8, -8);
        SpriteDrawable_SelectTileFrame(sprite, sprite->resource, 12);
        sprite->flags |= 1;
        sprite->priority = 2;
    }
}
