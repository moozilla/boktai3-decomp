#include "scene/collision/contact_swarm.h"
#include "player/player.h"
#include "engine/video/display_sprite_playback.h"
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
extern void (*const gUnk_08605920[])(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
u32 CountBats_Update(struct ContactSwarm *pool)
{
    if (pool->activeMask) {
        if (gPlayers[0])
            pool->target = *(const struct ContactPosition *)&gPlayers[0]->position;
        if (pool->fadeActive) {
            void *first = gPaletteBankData + 0x5A60;
            void *second = gPaletteBankData + 0x5AE0;
            Palette_InterpolateBanks(pool->palette, first, second, pool->fadeLevel, 5);
            if (!pool->fadeReverse) {
                if (++pool->fadeLevel > 30) {
                    pool->fadeLevel = 31;
                    pool->fadeReverse = 1;
                }
            } else {
                u32 value = (u8)pool->fadeLevel - 1;
                u32 zero = 0;
                pool->fadeLevel = value;
                if ((s8)value <= 0) {
                    pool->fadeLevel = zero;
                    pool->fadeReverse = zero;
                    pool->fadeActive = zero;
                }
            }
        }
        pool->resourceParity = pool->frame & 1;
        {
            struct ContactSwarmSlot *slot = pool->slots;
            s32 index;
            for (index = 0; index < 16; index++, slot++) {
                if (pool->activeMask & (1 << index)) {
                    gUnk_08605920[slot->state](pool, slot, index);
                    PlayerCue_StepSequenceEnded(&slot->display.animation.draw.sprite,
                                                &slot->display.animation.sequence);
                }
            }
        }
        pool->frame++;
    }
    return 0;
}
