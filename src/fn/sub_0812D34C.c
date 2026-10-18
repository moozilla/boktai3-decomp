#include "global.h"
#define SceneSlot_Remove sub_0812D34C
struct SceneSlot { u8 unknown[0x96]; u8 state; s8 index; u16 duration; u8 pad9a[2]; u16 timer; u8 pad9e; u8 reset; };
struct SceneSlotPool { u8 unknown[0x38]; u32 occupied; struct SceneSlot slots[8]; };
void Sprite_Remove(void *);
void sub_0812D34C(struct SceneSlotPool *pool, struct SceneSlot *slot)
{
    if (slot->reset) {
        slot->timer = 0;
        Sprite_Remove(slot);
        pool->occupied &= ~(1 << slot->index);
        slot->index = -1;
        slot->state = 0;
        slot->reset = 0;
    }
}
