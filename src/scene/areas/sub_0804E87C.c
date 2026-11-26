#include "scene/areas/area_effect.h"
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
void AreaEffect_HideSprites(struct AreaEffectManager *manager)
{
    if (manager->visible == 1) {
        s32 i;
        manager->visible = 0;
        for (i = 0; i < manager->count; i++) {
            if (manager->entries[i].hasSprite)
                manager->entries[i].spriteFlags |= 1;
        }
    }
    AreaEffect_AccumulatePlayerResource(manager, tst(&manager->player->statusFlags, 0x20) ? 2 : 0);
}
