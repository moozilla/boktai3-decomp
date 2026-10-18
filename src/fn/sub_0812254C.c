#define gSpriteEffectUpdates gUnk_086106E4
#include "global.h"
struct SpriteEffectSlot {
    u8 unknown[0xd6];
    u8 state;
    s8 index;
};
struct SpriteEffectPool {
    u8 unknown[0x34];
    u32 used;
    struct SpriteEffectSlot slots[8];
};
extern void *gUnk_086106E4[];
s32 sub_08249240(void *, void *, void *);
static inline u8 IsUsed(struct SpriteEffectPool *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
s32 SpriteEffectPool_Update(struct SpriteEffectPool *p)
{
    struct SpriteEffectSlot *slots = p->slots;
    if (p->used) {
        s32 i;
        for (i = 0; i < 8; i++) {
            if (IsUsed(p, i)) {
                struct SpriteEffectSlot *s = (void *)(i * sizeof(*slots) + (u32)slots);
                sub_08249240(p, s, gSpriteEffectUpdates[s->state]);
            }
        }
    }
    return 0;
}
