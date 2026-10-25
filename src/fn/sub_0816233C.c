#include "global.h"

#ifdef NONMATCHING
// fuzzy: 79.1%
// fix.py best (sweep after the inlined-insn fix)
// confidence=low: updates attack particles and advances their animation or lifetime.
#define PlayerAttackEffect_UpdateParticles sub_0816233C
// Open: inactive-path sharing, animation-step allocation, and spiral-vector temporaries.
#include "contact_swarm.h"
extern void (*const gUnk_08610E2C[])(u8 *);
extern const s16 gUnk_086149C4[];
struct Vec {
    u16 x, y, z, pad;
};
static inline s32 Divide4096(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline u32 Step(struct ContactSwarmSprite *draw, struct DisplaySequenceState *frames,
                       u32 one)
{
    struct ContactSwarmSprite *sprite = draw;
    struct DisplaySequenceState *state = frames;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, bit;
    u32 duration;
    u32 flags;
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
        u32 maskBit = one;
        bit = maskBit;
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
        u32 first;
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
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;

        mask = temp;

        /* Keep the live mask distinct from its source without binding r7. */

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
                s32 weight = state->frameWeight;
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
    return ended;
}

struct ParticleSlot {
    u32 life, sequence, mode;
    s32 angle;
};
struct ParticleEffect {
    u8 gap[0xc4];
    struct ContactSwarmSprite sprites[16];
    struct DisplaySequenceState sequences[16];
    struct ParticleSlot slots[16];
    u32 frame, unknown588;
    u8 *direction, *rotation, *owner;
    u32 previous, current, element;
    void *resource;
};
inline static u32 Step_1_1(struct ContactSwarmSprite *sprite, struct DisplaySequenceState *state,
                           struct ContactSwarmSprite *draw, struct DisplaySequenceState *frames,
                           u32 one)
{
    const volatile u16 *command;
    u32 ended;
    u32 bit;
    u32 duration;
    u32 flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = one;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        u32 first;
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

void PlayerAttackEffect_UpdateParticles(struct ParticleEffect *p)
{
    struct Vec delta;
    register s32 i;
    struct Vec *motion;
    struct ContactSwarmSprite *sprite;
    Callback_InvokeActor(p, gUnk_08610E2C[p->element]);
    p->previous = p->current;
    p->current = 0;
    motion = &delta;
    for (i = 0, sprite = p->sprites; i < 16; i++, sprite++) {
        struct ParticleSlot *slot = &p->slots[i];
        u32 old = slot->life;
        if (old == 0)
            sprite->flags |= 1;
        else {
            u32 time = old - 1;
            slot->life = time;
            if (slot->mode == 0) {
                u32 frame = 0;
                if (time == 0) {
                    slot->life = frame;
                    sprite->flags |= 1;
                } else {
                    frame = old - 2;
                    if (frame > 2)
                        frame = 2;
                    sprite->frame = slot->sequence + frame;
                }
            } else if (Step_1_1(sprite, &p->sequences[i], sprite, &p->sequences[i], 1)) {
                sprite->flags |= 1;
                slot->life = 0;
            } else {
                s32 angle;
                slot->life = 5;
                if (*p->rotation)
                    angle = slot->angle - 60;
                else
                    angle = slot->angle + 60;
                slot->angle = angle;
                delta.x = Divide4096(gUnk_086149C4[(angle + 64) & 255] * 16);
                delta.y = 0;
                delta.z = Divide4096(gUnk_086149C4[angle & 255] * 16);
                sprite->position.x += motion->x;
                sprite->position.y += motion->y;
                sprite->position.z += motion->z;
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0816233C);
#endif
