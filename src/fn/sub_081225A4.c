#include "global.h"
#include "ram.h"
struct SpriteEffectSlot {
    u8 sprite[0x90];
    u8 auxiliary[0x46];
    u8 state;
    s8 index;
};
struct SpriteEffectPool {
    u8 unknown[0x34];
    u32 used;
    struct SpriteEffectSlot slots[8];
};
void Sprite_Remove(void *);
void Shadow_Destroy(void *);
static inline u8 IsUsed(struct SpriteEffectPool *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
s32 SpriteEffectPool_RemoveSlots(struct SpriteEffectPool *p)
{
    s32 i = 0;
    for (; i < 8; i++) {
        if (IsUsed(p, i)) {
            struct SpriteEffectSlot *slot = &p->slots[i];
            Sprite_Remove(slot);
            Shadow_Destroy(slot->auxiliary);
        }
    }
    return (s32)(gSpriteEffectPool = 0);
}
