// MATCHING_CONCESSION: register-pinning
#include "route_sprite_actor.h"
void RouteSpriteActor_RunEventScript(struct RouteSpriteActor *);
void FragmentBurst_Create(struct ContactPosition *, u32, u32, u32, u32, u32, void *);
union EmissionPosition {
    struct ContactPosition position;
    struct ContactPackedVector packed;
};
static inline u32 NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}
void RouteSpriteActor_TickEmitting(struct RouteSpritePool *pool, struct RouteSpriteActor *input)
{
    struct RouteSpriteActor *actor = input;
    register struct RouteSpriteActor *flagsOwner asm("r9");
    struct DisplaySequenceState *sequence;
    u8 *entered;
    u16 *frames;
    u32 ended;
    flagsOwner = actor;
    sequence = &actor->sequence;
    entered = &actor->entered;
    if (*entered) {
        Anim_SetSequence(sequence, pool->sequenceResource, 0xE, 0, 0);
        actor->frames = 0;
        *entered = 0;
    }
    {
        struct ContactPosition *saved = &actor->savedPosition;
        struct ContactPosition position;
        struct RouteSpriteActor *dst;
        position = *saved;
        dst = actor;
        dst->sprite.position = position;
        {
            register u16 *low asm("r0") = &actor->frames;
            frames = low;
            if (*low <= 9) {
                const u16 *table = gRandomTable;
                u32 *index;
                register u32 next asm("r1");
                u32 mask;
                u32 random;
                u32 value;
                u32 spread;
                struct RouteSpriteActor *jitterDst;
                index = &gRandomIndex;
                next = *index + 1;
                mask = 1023;
                next &= mask;
                *index = next;
                random = table[next];
                value = (u16)saved->x;
                value -= 48;
                spread = 63;
                random &= spread;
                value += random;
                jitterDst = actor;
                jitterDst->sprite.position.x = value;
                next = (next + 1) & mask;
                *index = next;
                {
                    u32 random = table[next];
                    u32 value = (u16)actor->savedPosition.z;
                    value -= 48;
                    random &= spread;
                    value += random;
                    jitterDst->sprite.position.z = value;
                }
            }
        }
    }
    {
        u16 *low = frames;
        if (*low == 4) {
            union EmissionPosition position;
            {
                struct RouteSpriteActor *low = actor;
                AnimData_SelectPaletteBank(*(void **)&low->sprite.unknown8[4], 0x143);
            }
            {
                struct RouteSpriteActor *low = actor;
                position.position = low->sprite.position;
            }
            position.packed.y -= 60;
            FragmentBurst_Create(&position.position, (u16)actor->amount, actor->kind, actor->parameter,
                         actor->option, 20, actor->context);
            RouteSpriteActor_RunEventScript(actor);
        }
    }
    {
        struct RouteSpriteActor *primaryLow;
        const volatile u16 *command; /* preserve observed packed-command reads */
        u32 coreEnded, duration, flags, bit;
        {
            u32 offset = sequence->index << 1;
            const volatile u16 *table;
            primaryLow = actor;
            table = primaryLow->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        primaryLow->sprite.frame = *command >> 6;
        {
            u32 input = sequence->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            {
                register u32 value asm("r0");
                if (first != (((*command & 0x30) >> 4) & bit)) {
                    struct RouteSpriteActor *low = actor;
                    value = low->sprite.flags;
                    value |= 4;
                } else {
                    register struct RouteSpriteActor *low asm("r1") = actor;
                    asm("" : "+r"(low));
                    value = low->sprite.flags;
                    value &= ~4;
                }
                {
                    struct RouteSpriteActor *low = actor;
                    low->sprite.flags = value;
                }
            }
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
    if (ended) {
        u16 *cycles = &actor->cyclesRemaining;
        (*cycles)--;
        {
            register struct RouteSpriteActor *low asm("r2") = flagsOwner;
            AnimData_SelectPaletteBank(*(void **)&low->sprite.unknown8[4], 0x140);
        }
        if (*cycles) {
            u32 offset = 0x152;
            u8 *state;
            state = (void *)((u32)actor + offset);
            *state = 2;
        } else {
            u32 offset = 0x152;
            u8 *state;
            state = (void *)((u32)actor + offset);
            *state = 5;
        }
        actor->entered = 1;
    } else {
        u16 *frames = &actor->frames;
        (*frames)++;
    }
}
