#include "global.h"
#include "ram.h"
#define gEffectSlotPool gSpriteBurstManager
struct EffectSlot {
    u8 unknown[0x30];
    u16 timer;
    u8 state;
    s8 index;
    u8 entered;
    u8 unknown35[3];
    u16 duration;
};
struct EffectSlotPool {
    u8 unknown[0x69C];
    u32 allocated;
};
void Effect_Remove(void *);
void EffectSlot_Release(void *unused, struct EffectSlot *slot)
{
    if (slot->entered)
        slot->entered = 0;
    {
        struct EffectSlotPool *pool = gSpriteBurstManager;
        s32 index = slot->index;
        pool->allocated &= ~(1 << index);
    }
    slot->state = 0;
    Effect_Remove(slot);
    slot->index = -1;
    slot->timer = 0;
    slot->duration = 0;
}
