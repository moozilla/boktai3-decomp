#include "global.h"
#ifdef NONMATCHING
// fuzzy: 80.8%
// fix.py best (sweep after the inlined-insn fix)
// confidence=low: steers and accelerates a linked effect toward a target until expiry.
#define LinkedEffect_FollowTarget sub_0812ACB0
// Open: turn-branch sharing, vector addresses, and remaining allocation differences.
// MATCHING_CONCESSION: register-pinning
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
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
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
        register u32 temp asm("r3");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
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
extern u32 gRandomIndex;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void Anim_SetSequence(struct Anim *, void *, u32, u32, u32);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void LinkedEffect_UpdateColliders(struct Effect *);
static inline u32 RandomValue(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + (u32)gUnk_0203B400);
}
static inline s32 Truncate(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
void LinkedEffect_SetVariant(void *, u32);

inline static u32 Step_0812ACB0(struct Sprite *sprite, struct Anim *state)
{
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        command = (const volatile u16 *)(((u32)table) + offset);
        table = state->commands;
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
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
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r3");
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
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

void LinkedEffect_FollowTarget(struct Pool *poolArg, struct Effect *p)
{
    register struct Pool *pool asm("ip") = poolArg;
    struct Sprite *sprite = &p->sprite;
    struct Anim *sequence = &p->anim;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->riseTimer = 60;
        p->frame = 0;
        p->entered = 0;
    }
    if (p->ended) {
        s32 value = (u8)RandomValue();
        if (value <= 63)
            Anim_SetSequence(sequence, pool->resource, 21, 0, 0);
        else if (value <= 77)
            Anim_SetSequence(sequence, pool->resource, 22, 0, 0);
        else
            Anim_SetSequence(sequence, pool->resource, 20, 0, 0);
        p->ended = 0;
    }
    if (p->delay)
        p->delay--;
    else {
        volatile u16 delta[4];
        struct Vec *target = p->target;
        struct Vec *position = &sprite->position;
        s32 turn;
        delta[0] = target->x - position->x;
        delta[1] = target->y - position->y;
        delta[2] = target->z - position->z;
        turn = Math_Atan2((s16)delta[0], (s16)delta[2]) - p->heading;
        if (turn > 128)
            turn -= 256;
        else if (turn < (-128)) {
            register s32 wrap asm("r2") = 128;
            asm("" : "+r"(wrap));
            turn += wrap << 1;
        }
        if (p->turnTimer) {
            s32 magnitude = turn;
            if (magnitude < 0)
                magnitude = -magnitude;
            if (magnitude > 2) {
                if (turn < 0)
                    p->heading -= (RandomValue() & 1) + 1;
                else
                    p->heading += (RandomValue() & 1) + 1;
                p->turnTimer--;
            } else
                p->turnTimer = 0;
        }
    }
    {
        u32 heading = p->heading;
        register u32 speed asm("ip") = p->speed;
        struct Vec *velocity = &p->velocity;
        velocity->x = Truncate(speed * gUnk_086149C4[(heading + 64) & 255]);
        velocity->y = 0;
        velocity->z = Truncate(speed * gUnk_086149C4[heading]);
    }
    if ((!Mod(p->frame, 10)) && (p->speed < p->maxSpeed))
        p->speed++;
    sprite->position.x += p->velocity.x;
    if (p->riseTimer) {
        sprite->position.y += Div(p->target->y - (sprite->position.y - 200), p->riseTimer);
        p->riseTimer--;
    }
    sprite->position.z += p->velocity.z;
    p->ended = Step_0812ACB0(sprite, sequence);
    LinkedEffect_UpdateColliders(p);
    p->frame++;
    if (p->lifetime)
        p->lifetime--;
    else {
        p->state = 3;
        p->entered = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0812ACB0);
#endif
