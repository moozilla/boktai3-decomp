#include "area_effect.h"
#define AreaEffect_UpdateResourceFromPlayerFlags sub_0804E730
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
    sub_0804E618(manager, mode);
}
