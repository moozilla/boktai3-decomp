#include "global.h"
#define EffectSlot_Release sub_08125E74
#define gEffectSlotPool gUnk_02000150
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
extern struct EffectSlotPool *gUnk_02000150;
void Effect_Remove(void *);
void sub_08125E74(void *unused, struct EffectSlot *slot)
{
    if (slot->entered) slot->entered = 0;
    {
        struct EffectSlotPool *pool = gUnk_02000150;
        s32 index = slot->index;
        pool->allocated &= ~(1 << index);
    }
    slot->state = 0;
    Effect_Remove(slot);
    slot->index = -1;
    slot->timer = 0;
    slot->duration = 0;
}
