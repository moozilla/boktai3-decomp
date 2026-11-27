#include "player/effects/recoil_puff_sprite.h"
#include "engine/ram.h"
void Effect_Remove(void *);
u32 RecoilPuffPool_DestroyTask(struct RecoilPuffPool *p)
{
    struct RecoilPuffSprite *e = p->entries;
    s32 i = 0x7;
    do {
        Effect_Remove(e);
        e++;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gRisingSpritePool = z;
    }
    return 0;
}
