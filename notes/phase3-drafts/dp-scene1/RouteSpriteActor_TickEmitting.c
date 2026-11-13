#include "route_sprite_actor.h"
#include "display_sequence_inline.h"
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
    struct RouteSpriteActor *flagsOwner;
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
            u16 *low = &actor->frames;
            frames = low;
            if (*low <= 9) {
                const u16 *table = gRandomTable;
                u32 *index;
                u32 next;
                u32 nextZ;
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
                nextZ = (next + 1) & mask;
                *index = nextZ;
                {
                    u32 random = table[nextZ];
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
        const u16 *command;
        u32 coreEnded, duration, flags, bit;
        {
            u32 offset = sequence->index << 1;
            const u16 *table;
            primaryLow = actor;
            table = primaryLow->sequence.commands;
            command = (const u16 *)((u32)table + offset);
        }
        primaryLow->sprite.frame = *command >> 6;
        {
            union DisplaySequenceControl control;
            u32 first;
            control.raw = sequence->flags;
            bit = 1;
            first = (u8)(control.raw & bit);
            {
                u32 value;
                if (first != (((*command & 0x30) >> 4) & bit)) {
                    struct RouteSpriteActor *low = actor;
                    value = low->sprite.flags;
                    value |= 4;
                } else {
                    struct RouteSpriteActor *low = actor;

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
            union DisplaySequenceControl control;
            u32 first, bit;
            control.raw = sequence->flags;
            bit = 2;
            first = (u8)(control.raw & bit);
            {
                u32 value;
                if (first != (((*command & 0x30) >> 4) & bit)) {
                    struct RouteSpriteActor *low = flagsOwner;
                    value = low->sprite.flags;
                    value |= 8;
                } else {
                    struct RouteSpriteActor *low = flagsOwner;
                    value = low->sprite.flags;
                    value &= ~8;
                }
                {
                    struct RouteSpriteActor *low = flagsOwner;
                    low->sprite.flags = value;
                }
            }
        }
        {
            u32 elapsed = sequence->elapsed + 1;
            u32 mask;
            u32 temp;
            u32 savedDuration;
            sequence->elapsed = elapsed;
            temp = 0xFFFF;

            mask = temp;
            /* Keep the live mask distinct from its source without binding r7. */

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
                        endFlag = 8;
                        endFlag &= flags;

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
                    const u16 *table;
                    table = sequence->commands;
                    command = (const u16 *)((u32)table + offset);
                }
                sequence->frameWeight = *command & 15;
                {
                    s32 weight = sequence->frameWeight;
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
            struct RouteSpriteActor *low = flagsOwner;
            AnimData_SelectPaletteBank(*(void **)&low->sprite.unknown8[4], 0x140);
        }
        if (*cycles) {
            u32 offset = 0x152;
            u8 *state;
            state = &actor->state;
            *state = 2;
        } else {
            u32 offset = 0x152;
            u8 *state;
            state = &actor->state;
            *state = 5;
        }
        actor->entered = 1;
    } else {
        u16 *frames = &actor->frames;
        (*frames)++;
    }
}
