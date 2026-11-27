#include "player/effects/recoil_puff_sprite.h"
#include "engine/ram.h"
s32 RecoilPuffPool_Init(struct RecoilPuffPool *s)
{
    sub_08237280(s);
    s->next = 0;
    gRisingSpritePool = (u32)s;
    return 0;
}
