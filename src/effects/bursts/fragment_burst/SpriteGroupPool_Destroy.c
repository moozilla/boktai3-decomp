#include "effects/bursts/fragment_burst.h"

void Sprite_Remove(void *);

struct FragmentBurstPool *SpriteGroupPool_Destroy(struct FragmentBurstPool *pool)
{
    s32 i, j;

    for (i = 0; i < 4; i++) {
        if (FragmentBurstPool_IsOccupied(pool, i)) {
            struct FragmentBurst *burst = &pool->bursts[i];

            for (j = 0; j < 4; j++) {
                if (burst->sprites[j].unknown4[0])
                    Sprite_Remove(&burst->sprites[j]);
            }
        }
    }
    gSpriteGroupPool = 0;
    return 0;
}
