#include "player/effects/recoil_puff_sprite.h"
#include "scene/collision/contact_position.h"
extern struct RecoilPuffPool *gRisingSpritePool;
void RecoilPuff_Spawn(struct ContactPosition *position)
{
    struct RecoilPuffSprite *p;
    if (!gRisingSpritePool)
        RecoilPuffPool_GetOrCreate();
    p = &gRisingSpritePool->entries[gRisingSpritePool->next];
    p->flags &= ~1;
    p->position = *position;
    p->timer = 0;
    p->active = 1;
    gRisingSpritePool->next++;
    if (gRisingSpritePool->next > 7)
        gRisingSpritePool->next = 0;
}
