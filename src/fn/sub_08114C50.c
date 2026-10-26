#include "global.h"
#include "ram.h"
#define gSlotRegistry gSceneCallbackPool
struct RegistrySpriteSlot {
    u32 owner;
    u8 pad4[0x5C];
    u8 sprite[4];
    u8 spriteActive;
    u8 pad65[0x67];
    u16 id;
    u8 unkCE;
    u8 active;
    u8 padD0[8];
};
struct SlotRegistry {
    u8 pad0[0x18];
    struct RegistrySpriteSlot slots[16];
};
void Sprite_Remove(void *);
void SlotRegistry_RemoveSprite(u32 index)
{
    struct SlotRegistry *registry = gSceneCallbackPool;
    if (registry && index <= 15) {
        if (registry->slots[index].active && registry->slots[index].spriteActive) {
            Sprite_Remove(registry->slots[index].sprite);
            registry->slots[index].active = 0;
            registry->slots[index].id = 0;
            registry->slots[index].owner = 0;
        }
    }
}
