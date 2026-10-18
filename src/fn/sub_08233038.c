#define TemporarySpritePool_Update sub_08233038
#include "global.h"
struct TemporarySpriteSlot {
    u8 active, unknown1;
    s16 lifetime;
    u8 sprite[0x48];
};
struct TemporarySpritePool {
    u8 task[0x18];
    struct TemporarySpriteSlot slots[16];
};
void Sprite_Remove(void *);
s32 sub_08233038(struct TemporarySpritePool *pool)
{
    struct TemporarySpriteSlot *slot = pool->slots;
    s32 remaining = 15;
    do {
        if (slot->active) {
            --slot->lifetime;
            if (slot->lifetime <= 0) {
                Sprite_Remove(slot->sprite);
                slot->active = 0;
            }
        }
        --remaining;
        ++slot;
    } while (remaining >= 0);
    return 0;
}
