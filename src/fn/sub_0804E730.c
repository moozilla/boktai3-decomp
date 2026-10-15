#include "area_effect.h"
void AreaEffect_UpdateResourceFromPlayerFlags(struct AreaEffectManager *manager)
{
    struct AreaEffectPlayer *player = manager->player;
    u32 mask = 32;
    u32 flags = player->flags;
    u32 mode;
    if (flags & mask) {
        mode = 2;
    } else {
        u32 negative = 0 - (flags & 16);
        mode = negative >> 31;
    }
    AreaEffect_AccumulatePlayerResource(manager, mode);
}
