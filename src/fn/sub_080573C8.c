// MATCHING_CONCESSION: register-pinning
#include "level_advance_display.h"
#define LevelAdvanceDisplay_DestroyManager sub_080573C8
s32 LevelAdvanceDisplay_DestroyManager(struct LevelAdvanceManager *manager)
{
    struct LevelAdvanceParticle *particle;
    s32 i;
    Sprite_Remove(&manager->sprite);
    particle = manager->particles;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(particle);
        particle++;
    }
    {
        register u32 zero asm("r1") = 0;
        gUnk_02000120 = (void *)zero;
    }
    return 0;
}
