// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct ParticleVector {
    u16 x, y, z, unused;
};
struct ParticleSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Projectile {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[10];
    struct ParticleVector position;
    u8 unknown24[8];
    struct ParticleSequence sequence;
    struct ParticleVector velocity;
    u8 collider[0x56];
    u8 active;
    s8 slot;
    u16 age;
    u8 enter, angle;
};
struct TerrainMap {
    u32 unknown0;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
void *TerrainModifiers_FindCellOverride(u32, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
static inline u32 Step(struct Projectile *const owner, struct ParticleSequence *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        {
            table = owner->sequence.commands;
        }
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
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
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        u8 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Preserve a separate working copy of the index mask. */
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
                s32 weight = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = speed * weight;
                speed >>= 6;
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
struct TimedSpritePool {
    u8 unknown0[0x34];
    void *resource;
};
void Anim_SetSequence(struct ParticleSequence *, void *, s32, s32, s32);
void TimedSprite_BlinkAndRemove(struct TimedSpritePool *, struct Projectile *);
static inline u8 *SelectDirection(u8 *angle, u8 *row, u8 *flip)
{
    register u8 *t0 asm("r0") = flip;
    u8 *f;
    u32 quadrant;
    quadrant = ((*angle + 0x60) >> 6) & 3;
    f = t0;
    if (quadrant > 1) {
        *f = 1;
        *row = 3 - quadrant;
    } else {
        *f = 0;
        *row = quadrant;
    }
    return f;
}
// confidence=low: initializes direction-dependent playback before the blinking removal state.
void TimedSprite_UpdateBlinkAnimation(struct TimedSpritePool *pool, struct Projectile *input)
{
    struct Projectile *const p = input;
    u8 direction, mirrored;
    struct Projectile *incoming = input;
    if (p->enter) {
        u8 *flip = SelectDirection(&incoming->angle, &direction, &mirrored);
        Anim_SetSequence(&p->sequence, pool->resource, 19, direction, *flip);
        p->age = 0;
        p->enter = 0;
    }
    Step(p, &p->sequence);
    TimedSprite_BlinkAndRemove(pool, p);
    {
        u16 *const age = &p->age;
        (*age)++;
    }
}
