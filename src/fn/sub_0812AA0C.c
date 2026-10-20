#include "global.h"
#ifdef NONMATCHING
// fuzzy: 83.8%
// Entry vector arithmetic and register allocation still differ.
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 visible, unknown5, palette, unknown7;
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 pad24[0x24];
};
struct Anim {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
static inline u32 Step(struct Sprite *sprite, struct Anim *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
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
        u32 maskBit = 1;
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

struct Effect {
    struct Sprite sprite;
    struct Anim anim;
    u8 pad58[0xA8];
    struct Vec *target;
    u8 pad104[0x68];
    struct Vec velocity;
    u16 unknown174;
    u8 state;
    s8 slot;
    u16 frame;
    u8 entered, speed, ended, heading;
    u16 unknown17E, lifetime, riseTimer, delay, turnTimer, maxSpeed;
};
struct Pool {
    u8 pad[0x18];
    void *resource;
    u32 allocated;
};
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void Anim_SetSequence(struct Anim *, void *, u32, u32, u32);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void LinkedEffect_UpdateColliders(struct Effect *);
static inline u32 RandomValue(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return gUnk_0203B400[index];
}
static inline s32 Truncate(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
void LinkedEffect_SetVariant(void *, u32);
// confidence=low: expands the linked effect while preparing an elevated travel target.
#define LinkedEffect_BeginRisingTravel sub_0812AA0C
void LinkedEffect_BeginRisingTravel(struct Pool *pool, struct Effect *p)
{
    if (p->entered) {
        u32 heading;
        u32 distance;
        struct Vec *velocity, *position;
        p->sprite.flags = (p->sprite.flags & ~1) | 2;
        p->ended = 0;
        heading = p->heading;
        distance = (u8)RandomValue();
        velocity = &p->velocity;
        distance += 64;
        velocity->x = Truncate(distance * gUnk_086149C4[(heading + 64) & 255]);
        velocity->y = 0;
        velocity->z = Truncate(distance * gUnk_086149C4[heading]);
        velocity = &p->velocity;
        position = &p->sprite.position;
        {
            u32 x = velocity->x;
            velocity->x = x + position->x;
        }
        velocity->y = velocity->y + position->y;
        velocity->z = velocity->z + position->z;
        p->velocity.y = p->sprite.position.y + 512;
        p->riseTimer = 15;
        p->frame = 0;
        p->entered = 0;
        p->sprite.scaleX = 1;
        p->sprite.scaleY = 1;
    }
    if (!Mod(p->frame, 8))
        LinkedEffect_SetVariant(&p->sprite.position, (u8)(p->heading + 128));
    if (p->riseTimer) {
        p->sprite.scaleX += Div(64 - p->sprite.scaleX, p->riseTimer);
        p->sprite.scaleY += Div(64 - p->sprite.scaleY, p->riseTimer);
        p->riseTimer--;
    }
    p->ended = Step(&p->sprite, &p->anim);
    if (p->ended && !p->riseTimer) {
        p->state = 2;
        p->sprite.flags &= ~2;
        p->entered = 1;
    } else
        p->frame++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0812AA0C);
#endif
