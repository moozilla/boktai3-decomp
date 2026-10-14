// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
#define ContactSwarm_Update sub_08044594
void sub_08221110(void *, void *, void *, s32, s32);
extern void (*const gUnk_08605920[])(struct ContactSwarm *, struct ContactSwarmSlot *, s32);
static inline void updatePlayback(struct ContactSwarmAnimation *animation,
                                  struct ContactSwarmSprite *spriteInput)
{
    struct ContactSwarmSprite *sprite = spriteInput;
    struct DisplaySequenceState *state = &animation->sequence;
    const volatile u16 *command;
    u32 flags, bit;
    u32 duration;
    {
        u32 offset = animation->sequence.index << 1;
        const volatile u16 *table;
        table = animation->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    animation->sprite.frame = *command >> 6;
    {
        u32 input = animation->sequence.flags;
        u32 first;
        first = 1;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & 1))
            animation->sprite.flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r2");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                } else {
                    state->index = index - 1;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                } else {
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed) {
                    u32 one = 1;
                    state->duration = one;
                }
            }
        }
    }
}
u32 ContactSwarm_Update(struct ContactSwarm *poolInput)
{
    struct ContactSwarm *pool = poolInput;
    if (poolInput->activeMask) {
        if (gUnk_02000580[0])
            pool->target = *(const struct ContactPosition *)&gUnk_02000580[0]->position;
        {
            u8 *fadeActive = &pool->fadeActive;
            if (*fadeActive) {
                u8 *bank = gUnk_030042E4;
                u32 firstOffset = 0x5A60;
                void *first = bank + firstOffset;
                void *second = bank + firstOffset + 0x80;
                void *palette;
                s8 *level;
                u8 *reverse;
                palette = pool->palette;
                level = &pool->fadeLevel;
                sub_08221110(palette, first, second, *level, 5);
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
        {
            register const struct ContactSwarm *current asm("r1") = pool;
            pool->resourceParity = current->frame & 1;
        }
        {
            struct ContactSwarmSlot *slot;
            struct ContactSwarmSlot *initialSlot;
            register u32 slotOffset asm("r2") = (u8 *)&pool->slots[0] - (u8 *)pool;
            struct ContactSwarmAnimation *sequence;
            s32 index;
            s32 initialIndex;
            s32 test;
            asm("" : "+r"(slotOffset));
            initialSlot = (void *)(slotOffset + (u32)pool);
            asm("" : "+r"(initialSlot));
            slot = initialSlot;
            initialIndex = 0;
            index = initialIndex;
            sequence = &pool->slots[0].animation;
            do {
                u32 mask = 1;
                register const struct ContactSwarm *current asm("r2");
                register s32 bitIndex asm("r2");
                bitIndex = index;
                asm("" : "+r"(bitIndex));
                mask <<= bitIndex;
                current = pool;
                asm("" : "+r"(current));
                if (current->activeMask & mask) {
                    register void (*const *table)(struct ContactSwarm *, struct ContactSwarmSlot *,
                                                  s32) asm("r1") = gUnk_08605920;
                    register const struct ContactSwarmSlot *currentSlot asm("r2");
                    currentSlot = slot;
                    asm("" : "+r"(currentSlot));
                    table[currentSlot->state](pool, slot, index);
                    updatePlayback(sequence, &slot->animation.sprite);
                }
                {
                    register u32 one asm("r1") = 1;
                    register u32 stride asm("r2");
                    index += one;
                    stride = sizeof(*slot);
                    sequence = (void *)((u8 *)sequence + stride);
                    slot = (void *)((u8 *)slot + stride);
                    test = index;
                }
            } while (test <= 15);
        }
        {
            register struct ContactSwarm *current asm("r1") = pool;
            current->frame++;
        }
    }
    return 0;
}
