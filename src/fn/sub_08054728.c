#include "hit_switch.h"
void sub_08225938(struct SceneBody *);
void sub_0821D6D0(struct SceneCellRef *);
s32 HitSwitch_DestroyManager(struct HitSwitchManager *manager)
{
    s32 i;
    for (i = 0; i < manager->count; i++) {
        struct HitSwitchEntry *entry = &manager->entries[i];
        if (entry->bodyEnabled)
            sub_08225938(&entry->body);
        Sprite_Remove(&entry->sprite);
        Collider_Remove(&entry->collider);
        if (entry->cellRegistered)
            sub_0821D6D0(&entry->cellRef);
    }
    gUnk_02000490 = 0;
    return 0;
}
