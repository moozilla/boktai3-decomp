#include "area_effect.h"
void AreaEffect_UpdateGraphicsId(struct AreaEffectManager *);
void AreaEffect_ShowEnabledSprites(struct AreaEffectManager *manager)
{
    if (!manager->visible) {
        s32 i;
        manager->visible = 1;
        for (i = 0; i < manager->count; i++) {
            if (manager->entries[i].hasSprite && manager->entries[i].enabled)
                manager->entries[i].spriteFlags &= ~1;
        }
    }
    AreaEffect_UpdateGraphicsId(manager);
    {
        struct AreaEffectPlayer *player = manager->player;
        u32 mask = 32;
        u32 flags = player->flags;
        s32 mode;
        if (flags & mask)
            mode = 2;
        else if (flags & 16)
            mode = 1;
        else
            mode = 0;
        AreaEffect_AccumulatePlayerResource(manager, mode);
    }
}
