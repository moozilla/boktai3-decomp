#include "area_effect.h"
#define AreaEffect_Destroy sub_0804EAFC
void Sprite_Remove(void *);
s32 AreaEffect_Destroy(struct AreaEffectManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        do {
            if (manager->entries[i].hasSprite)
                Sprite_Remove(&manager->entries[i].spriteFlags);
            i++;
        } while (i < manager->count);
    }
    gUnk_0200010C = 0;
    return 0;
}
