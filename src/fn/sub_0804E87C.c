#include "area_effect.h"
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
void AreaEffect_HideSprites(struct AreaEffectManager *manager)
{
    if (manager->visible == 1) {
        s32 i;
        manager->visible = 0;
        i = 0;
        if (i < manager->count) {
            u32 m = 1;
            u8 *fp = &manager->entries[0].hasSprite;
            u8 *e = (u8 *)manager;
            u8 *np = &manager->count;
            do {
                if (*fp != 0)
                    *(u32 *)(e + 0x34) |= m;
                fp += sizeof(struct AreaEffectEntry);
                e += sizeof(struct AreaEffectEntry);
                i++;
            } while (i < *np);
        }
    }
    AreaEffect_AccumulatePlayerResource(manager, tst(&manager->player->statusFlags, 0x20) ? 2 : 0);
}
