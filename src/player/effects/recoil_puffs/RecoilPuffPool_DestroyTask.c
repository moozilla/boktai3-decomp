#include "player/effects/recoil_puff_sprite.h"
#include "engine/ram.h"
void Effect_Remove(struct RecoilPuffSprite *);

u32 RecoilPuffPool_DestroyTask(struct RecoilPuffPool *pool)
{
    s32 i;

    for (i = 0; i < 8; i++)
        Effect_Remove(&pool->entries[i]);
    gRisingSpritePool = 0;
    return 0;
}
