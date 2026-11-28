#include "effects/bursts/charged_burst.h"

void ParticleSprite_Initialize(struct ChargedBurstSprite *, void *, s32);
void Effect_SetOrigin(struct ChargedBurstSprite *, s32, s32);
void SpriteDrawable_SelectTileFrame(struct ChargedBurstSprite *, void *, s32);
void Effect_SetPalette(struct ChargedBurstSprite *, s32);

void ChargedBurst_InitParticles(struct ChargedBurst *burst)
{
    s32 i;

    burst->sparkResource = AnimData_FindEntry(0x1c1e);
    for (i = 0; i < 8; i++) {
        struct ChargedBurstSprite *sprite = &burst->sparks[i].sprite;

        ParticleSprite_Initialize(sprite, burst->sparkResource, 1);
        Effect_SetOrigin(sprite, -4, -4);
        SpriteDrawable_SelectTileFrame(sprite, burst->sparkResource, 8);
        Effect_SetPalette(sprite, 1);
        sprite->unk_0F = 2;
        sprite->unk_10 = 236;
        burst->sparks[i].active = 0;
        burst->sparks[i].timer = 0;
    }
}
