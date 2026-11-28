#include "effects/bursts/fragment_burst.h"

void Sprite_Remove(void *);
void FragmentBurst_RunCompletionScript(struct FragmentBurst *);

void SpriteGroup_Release(struct FragmentBurstPool *pool, struct FragmentBurst *burst)
{
    s32 i;

    if (burst->enter) {
        pool->occupied &= ~(1 << burst->slot);
        burst->state = 0;
        for (i = 0; i < 4; i++) {
            if (burst->sprites[i].unknown4[0])
                Sprite_Remove(&burst->sprites[i]);
        }
        burst->slot = -1;
        burst->age = 0;
        FragmentBurst_RunCompletionScript(burst);
        burst->enter = 0;
    }
}
