#define SpriteEffectPool_AllocateSlot sub_08122020
#include "global.h"
struct SpriteEffectSlot { u8 unknown[0xd7]; s8 index; };
struct SpriteEffectPool { u8 unknown[0x34]; u32 used; struct SpriteEffectSlot slots[8]; };
static inline u8 IsUsed(struct SpriteEffectPool *p, u32 index) { if (p->used & (1U << index)) return 1; return 0; }
struct SpriteEffectSlot *sub_08122020(struct SpriteEffectPool *p)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!IsUsed(p, i)) {
            struct SpriteEffectSlot *slot = &p->slots[i];
            p->used |= 1U << i;
            slot->index = i;
            return slot;
        }
    }
    return 0;
}
