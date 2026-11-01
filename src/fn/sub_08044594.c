#include "contact_swarm.h"
#include "player.h"
#include "display_sequence_inline.h"
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
extern void (*const gUnk_08605920[])(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
static inline void updatePlayback(struct ContactSwarmAnimation *animation,
                                  struct ContactSwarmSprite *sprite)
{
    struct DisplaySequenceState *state = &animation->sequence;
    const u16 *command;
    {
        u32 offset = state->index << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    animation->draw.sprite.frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            animation->draw.sprite.flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}
u32 ContactSwarm_Update(struct ContactSwarm *pool)
{
    if (pool->activeMask) {
        if (gPlayers[0])
            pool->target = *(const struct ContactPosition *)&gPlayers[0]->position;
        {
            u8 *fadeActive = &pool->fadeActive;
            if (*fadeActive) {
                u8 *bank = gPaletteBankData;
                u32 firstOffset = 0x5A60;
                void *first = bank + firstOffset;
                void *second = bank + firstOffset + 0x80;
                void *palette;
                s8 *level;
                u8 *reverse;
                palette = pool->palette;
                level = &pool->fadeLevel;
                Palette_InterpolateBanks(palette, first, second, *level, 5);
                reverse = &pool->fadeReverse;
                if (!*reverse) {
                    if (++*level > 30) {
                        *level = 31;
                        *reverse = 1;
                    }
                } else {
                    u32 value = (u8)*level - 1;
                    u32 zero = 0;
                    *level = value;
                    if ((s8)value <= 0) {
                        *level = zero;
                        *reverse = zero;
                        *fadeActive = zero;
                    }
                }
            }
        }
        pool->resourceParity = pool->frame & 1;
        {
            struct ContactSwarmSlot *slot = pool->slots;
            s32 index;
            for (index = 0; index <= 15; index++, slot++) {
                if (pool->activeMask & (1 << index)) {
                    gUnk_08605920[slot->state](pool, slot, index);
                    updatePlayback(&slot->display.animation, &slot->display.animation.draw.sprite);
                }
            }
        }
        pool->frame++;
    }
    return 0;
}
