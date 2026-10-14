// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
void ContactActor_UpdateDisplay(struct ContactActor *slot)
{
    register struct ContactActor *owner asm("r12") = slot;
    struct DisplaySequenceSprite *sprite = &owner->sprite;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 ended, duration, flags;
    u32 bit;
    struct ContactPosition *destination;
    destination = (struct ContactPosition *)((u8 *)owner + 0x100);
    {
        register const struct ContactActor *source asm("r3") = owner;
        *destination = source->position;
    }
    state = &owner->sequence;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        bit = 1;
        first = bit;
        asm("" : "+r"(first));
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
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
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
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
                    ended = 1;
                } else {
                    ended = 0;
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
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    {
        register struct ContactActor *current asm("r3") = owner;
        s32 diff;
        current->finished = ended;
        diff = current->position.y - current->targetHeight;
        if (diff <= -4) {
            current->position.y += 4;
        } else if (diff > 3) {
            current = owner;
            current->position.y -= 4;
        } else {
            struct ContactActor *base = owner;
            base->position.y = base->targetHeight;
        }
    }
    {
        const s16 *table = gUnk_086149C4;
        register struct ContactActor *reader asm("r2");
        u32 phase;
        s32 wave;
        s32 offset;
        reader = owner;
        phase = reader->bobPhase;
        wave = table[phase] * 32;
        asm("" : : "r"(reader));
        if (wave >= 0)
            offset = wave >> 12;
        else
            offset = -((-wave) >> 12);
        *(s16 *)((u8 *)sprite + 0x1E) += offset;
        {
            struct ContactActor *base = owner;
            base->bobPhase += 4;
        }
    }
}
