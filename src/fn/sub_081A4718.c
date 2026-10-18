#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define EffectSpritePool_InitializeEntries sub_081A4718
struct EffectSpriteEntry {
    u32 flags;
    u8 unknown4[0x48];
    u32 owner;
    u16 state;
    u8 unknown52[6];
    u8 frame, mode;
    u8 unknown5a[2];
};
struct EffectSpritePool {
    u8 unknown0[0x18];
    u8 animation[0x44];
    struct EffectSpriteEntry entries[32];
};
void Sprite_Init(void *, void *, u32);
s32 sub_081A4718(struct EffectSpritePool *pool)
{
    struct EffectSpriteEntry *entry = pool->entries;
    u32 zero = 0;
    s32 i = 31;
    do {
        Sprite_Init(entry, pool->animation, 0);
        entry->flags |= 1;
        entry->owner = zero;
        entry->frame = zero;
        entry->mode = zero;
        entry->state = zero;
        entry++;
    } while (--i >= 0);
    return 0;
}
