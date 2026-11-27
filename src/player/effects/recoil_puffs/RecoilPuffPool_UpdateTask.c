#include "player/effects/recoil_puff_sprite.h"
u32 RecoilPuffPool_UpdateTask(struct RecoilPuffPool *s)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (s->entries[i].active)
            RecoilPuff_Update(&s->entries[i]);
    }
    return 0;
}
