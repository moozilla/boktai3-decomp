// MATCHING_CONCESSION: register-pinning
#include "route_sprite_playback.h"
#define RouteSpriteActor_TickPausing sub_0806DB20
static inline s32 Truncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
void RouteSpriteActor_TickPausing(struct RouteSpritePool *pool, struct RouteSpriteActor *input)
{
    register struct RouteSpriteActor *actor asm("r6") = input;
    register struct RouteSpriteActor *flagsOwner asm("r8") = actor;
    struct DisplaySequenceState *sequence;
    u8 *entered;
    u32 ended;
    sequence = &actor->sequence;
    entered = &actor->entered;
    if (*entered) {
        {
            void *data = pool->sequenceResource;
            u32 zero;
            zero = 0;
            Anim_SetSequence(sequence, data, 0xE, 0, zero);
            actor->frames = zero;
        }
        *entered = 0;
        actor->angle = 0;
    }
    {
        const volatile u16 *command; /* preserve observed packed-command reads */
        u32 coreEnded, duration, flags, bit;
        {
            u32 offset = sequence->index << 1;
            const volatile u16 *table;
            table = actor->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        actor->sprite.frame = *command >> 6;
        {
            u32 input = sequence->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            if (first != (((*command & 0x30) >> 4) & bit))
                actor->sprite.flags |= 4;
            else
                actor->sprite.flags &= ~4;
        }
        {
            u32 input = sequence->flags;
            u32 bit;
            register u32 first asm("r1");
            bit = 2;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            asm("" : "+r"(first));
            first = (u8)first;
            {
                register u32 value asm("r0");
                if (first != (((*command & 0x30) >> 4) & bit)) {
                    register struct RouteSpriteActor *low asm("r1") = flagsOwner;
                    value = low->sprite.flags;
                    value |= 8;
                } else {
                    register struct RouteSpriteActor *low asm("r1") = flagsOwner;
                    value = low->sprite.flags;
                    value &= ~8;
                }
                {
                    register struct RouteSpriteActor *low asm("r2") = flagsOwner;
                    low->sprite.flags = value;
                }
            }
        }
        {
            u32 elapsed = sequence->elapsed + 1;
            u32 mask;
            register u32 temp asm("r1");
            register u32 savedDuration asm("r3");
            sequence->elapsed = elapsed;
            temp = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            /* Keep the live mask distinct from its source without binding r7. */
            asm("" : "+&r"(mask) : "r"(temp));
            duration = sequence->duration;
            elapsed = (u16)elapsed;
            savedDuration = duration;
            if (elapsed >= duration) {
                sequence->elapsed = 0;
                flags = sequence->flags;
                if (flags & 4) {
                    u32 index = sequence->index;
                    if (index == 0) {
                        u32 hold = 8;
                        hold &= flags;
                        if (hold) {
                            sequence->elapsed = savedDuration;
                        } else {
                            sequence->index = sequence->frameCount - 1;
                        }
                        coreEnded = 1;
                    } else {
                        sequence->index = index - 1;
                        coreEnded = 0;
                    }
                } else {
                    u32 index = sequence->index + 1;
                    u32 frameCount;
                    u32 endFlag;
                    sequence->index = index;
                    frameCount = sequence->frameCount;
                    if ((index & mask) >= frameCount) {
                        endFlag = flags & 8;
                        asm("" : "+r"(endFlag));
                        endFlag = (u8)endFlag;
                        if (endFlag) {
                            sequence->elapsed = duration;
                            {
                                u32 lastIndex = frameCount - 1;
                                sequence->index = lastIndex;
                            }
                        } else {
                            sequence->index = 0;
                        }
                        coreEnded = 1;
                    } else {
                        coreEnded = 0;
                    }
                }
                {
                    u32 offset = sequence->index << 1;
                    const volatile u16 *table;
                    table = sequence->commands;
                    command = (const volatile u16 *)((u32)table + offset);
                }
                sequence->frameWeight = *command & 15;
                {
                    register s32 weight asm("r1") = sequence->frameWeight;
                    s32 speed;
                    speed = sequence->speed;
                    speed = (speed * weight) >> 6;
                    sequence->duration = speed;
                    if (!(u8)speed)
                        sequence->duration = 1;
                }
            } else {
                coreEnded = 0;
            }
        }

        ended = coreEnded;
    }
    {
        register u32 offset asm("r2") = 0x15A;
        s8 *angle;
        asm("" : "+r"(offset));
        angle = (void *)((u32)actor + offset);
        if (*angle >= 0) {
            const s16 *table = gUnk_086149C4;
            s32 amount;
            amount = Truncate12(table[((u8)*angle + 64) & 255] * 64);
            {
                u32 offset = 0x14A;
                u16 *savedY;
                u32 value;
                register struct RouteSpriteActor *low asm("r1");
                savedY = (void *)((u32)actor + offset);
                value = *savedY;
                value -= amount;
                low = flagsOwner;
                low->sprite.position.y = value;
                offset += 0x10;
                {
                    u8 *angle = (void *)((u32)actor + offset);
                    *angle += 4;
                }
            }
        }
    }
    {
        register u32 offset asm("r0") = 0x154;
        u16 *frames;
        u32 count;
        asm("" : "+r"(offset));
        frames = (void *)((u32)actor + offset);
        count = *frames;
        if (count <= 59)
            *frames = count + 1;
        else if (ended) {
            {
                register u32 offset asm("r2") = 0x152;
                register u8 *state asm("r1");
                state = (void *)((u32)actor + offset);
                *state = 4;
            }
            {
                register u32 offset asm("r0") = 0x156;
                register u8 *entered asm("r1");
                entered = (void *)((u32)actor + offset);
                *entered = 1;
            }
        }
    }
}
