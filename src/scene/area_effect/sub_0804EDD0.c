#include "scene/area_effect/area_effect_link.h"
#include "scene/actor/scene_actor_damage_visibility.h"
static inline u8 AreaEffect_GetLinkValue(s32 index)
{
    if (!gLinkSession || index < 0)
        return 0;
    return gLinkSession->playerValues[index];
}
void AreaEffect_UpdateLinkVisibility(struct AreaEffectManager *manager)
{
    s32 i;
    u32 value = AreaEffect_GetLinkValue(SceneActorDamage_LocalPlayer());
    if (value) {
        if (!manager->visible) {
            manager->visible = 1;
            for (i = 0; i < manager->count; i++) {
                if (manager->entries[i].hasSprite && manager->entries[i].enabled)
                    manager->entries[i].spriteFlags &= ~1;
            }
        }
    } else {
        if (manager->visible == 1) {
            manager->visible = 0;
            for (i = 0; i < manager->count; i++) {
                if (manager->entries[i].hasSprite)
                    manager->entries[i].spriteFlags |= 1;
            }
        }
    }
}
