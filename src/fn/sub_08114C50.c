#include "scene_callback_pool.h"
#include "ram.h"
void Sprite_Remove(void *);
void SlotRegistry_RemoveSprite(u32 index)
{
    struct SceneCallbackPool *registry = gSceneCallbackPool;
    if (registry && index <= 15) {
        if (registry->slots[index].active && registry->slots[index].sprite.unknown4[0]) {
            Sprite_Remove(&registry->slots[index].sprite);
            registry->slots[index].active = 0;
            registry->slots[index].id = 0;
            registry->slots[index].callback = 0;
        }
    }
}
