// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#include "engine_globals.h"
struct Vec {
    s16 x, y, z, pad;
};
struct FeatherSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct Vec position;
    u8 unknown24[8];
    struct FeatherSequence sequence;
    u8 unknown3C[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u16 blinkAge;
    u8 enter, angle;
    u32 effectFlags, variant;
};
struct FeatherPool {
    u8 task[0x18];
    u8 graphics[0x1c];
    void *sequences;
    u32 used;
};
void Sprite_Remove(void *);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
static inline void SelectDirection(u32 angle, u8 *row, u8 *flip)
{
    u32 quadrant = ((((s32)(angle >> 24)) + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
}

extern u32 gUnk_02000560;
struct FeatherPool *FeatherEffectPool_GetOrCreate(void);
struct Feather *FeatherEffectPool_AllocEntry(struct FeatherPool *);
// confidence=low: creates a directional feather effect with palette and scatter variants.
struct Feather *FeatherEffectPool_CreateDirectional(struct Vec *position, u32 angle, u32 flags,
                                                    u32 variant)
{
    struct FeatherPool *pool = gFadingSpritePool;
    struct Feather *p;
    struct Feather *sprite;
    u8 row, flip;
    asm("" : : "r"(angle));
    asm("" : : "r"(angle));
    asm("" : : "r"(angle));
    asm("" : : "r"(angle));
    asm("" : : "r"(angle));
    if (!pool) {
        pool = FeatherEffectPool_GetOrCreate();
        if (!pool)
            return 0;
    }
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return 0;
    }
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    p = FeatherEffectPool_AllocEntry(pool);
    if (!p)
        return 0;
    p->age = 0;
    p->angle = angle;
    p->enter = 1;
    p->state = 2;
    sprite = p;
    sprite->position = *position;
    SelectDirection(angle << 24, &row, &flip);
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(&p->sequence, pool->sequences, 1, row, flip);
    p->variant = variant;
    if (!variant) {
        AnimData_SelectPaletteBank(sprite->graphics, 0x278);
        p->effectFlags = 0x200004;
    } else {
        AnimData_SelectPaletteBank(sprite->graphics, 0x277);
        p->effectFlags = 0x400008;
    }
    {
        struct FeatherSequence *state = &p->sequence;
        struct Feather *base = p;
        const volatile u16 *command; /* preserve observed packed-command reads */
        u32 ended, duration, flags, bit;
        {
            u32 offset = state->index << 1;
            const volatile u16 *table;
            table = base->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        sprite->frame = *command >> 6;
        {
            u32 input = state->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));
            first = bit;
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
            u32 mask;
            register u32 temp asm("r1");
            register u32 savedDuration asm("r3");
            state->elapsed = elapsed;
            temp = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            /* Keep the live mask distinct from its source without binding r7. */
            asm("" : "+&r"(mask) : "r"(temp));
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
    }
    {
        u32 combined = sprite->flags | 1;
        sprite->priority = 1;
        {
            register u32 input asm("r2") = flags;
            asm("" : : "r"(input));
            combined |= input;
        }
        sprite->flags = combined;
    }
    return p;
}
