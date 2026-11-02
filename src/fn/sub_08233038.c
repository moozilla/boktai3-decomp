#include "transient_sprite_pool.h"
void Sprite_Remove(void *);
s32 TemporarySpritePool_Update(struct TemporarySpritePool *pool)
{
    struct TemporarySpriteSlot *slot = pool->slots;
    s32 remaining = 15;
    do {
        if (slot->active) {
            --slot->lifetime;
            if (slot->lifetime <= 0) {
                Sprite_Remove(&slot->sprite);
                slot->active = 0;
            }
        }
        --remaining;
        ++slot;
    } while (remaining >= 0);
    return 0;
}
