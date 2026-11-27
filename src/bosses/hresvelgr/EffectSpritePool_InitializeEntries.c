#include "global.h"
#include "bosses/hresvelgr/effect_sprite_entry_layout.h"
/* Effect pool family names and field interpretations are tentative. */
struct EffectSpritePool {
    u8 unknown0[0x18];
    u8 animation[0x44];
    struct EffectSpriteEntry entries[32];
};
void Sprite_Init(void *, void *, u32);
s32 EffectSpritePool_InitializeEntries(struct EffectSpritePool *pool)
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
