#include "repeated_effect.h"
void SceneBody_Unregister(void *);
void Sprite_Remove(void *);
void Collider_Remove(void *);
s32 RepeatedEffect_DestroyManager(struct RepeatedEffectManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        void *collider = &manager->entries[0].collider;
        void *sprite = &manager->entries[0].sprite;
        void *body = manager->entries[0].body;
        u8 *registered = &manager->entries[0].bodyRegistered;
        do {
            if (*registered)
                SceneBody_Unregister(body);
            Sprite_Remove(sprite);
            Collider_Remove(collider);
            collider = (u8 *)collider + 0xFC;
            sprite = (u8 *)sprite + 0xFC;
            body = (u8 *)body + 0xFC;
            registered += 0xFC;
            i++;
        } while (i < manager->count);
    }
    gRepeatedEffectManager = 0;
    return 0;
}
