// scope: write LinkedEffect_FollowTarget here
#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// confidence=low: steers and accelerates a linked effect toward a target until expiry.
#define LinkedEffect_FollowTarget sub_0812ACB0
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
        register u32 maskBit = 1;
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
extern const s16 gUnk_086149C4[];
void Anim_SetSequence(struct Anim *, void *, u32, u32, u32);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void LinkedEffect_UpdateColliders(struct Effect *);
static inline s32 RandomValue(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + RANDOM_TABLE_ADDR);
}
static inline s32 Truncate(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

static inline void Vec_Subtract(struct Vec *out, const struct Vec *a, const struct Vec *b)
{
    out->x = b->x - a->x;
    out->y = b->y - a->y;
    out->z = b->z - a->z;
}

static inline void Polar(struct Vec *out, u32 heading, u32 speed)
{
    out->x = Truncate(speed * gUnk_086149C4[(heading + 64) & 255]);
    out->y = 0;
    out->z = Truncate(speed * gUnk_086149C4[heading]);
}

static inline u16 RandomByte(void)
{
    u32 *seed = &gRandomIndex;
    const u16 *table;
    u32 index = (*seed + 1) & 1023;
    u32 offset;
    *seed = index;
    offset = index << 1;
    table = gRandomTable;
    return (u8) * (const u16 *)(offset + (u32)table);
}

void LinkedEffect_FollowTarget(struct Pool *poolArg, struct Effect *p)
{
    struct Pool *pool = poolArg;
    struct Sprite *sprite = &p->sprite;
    struct Anim *sequence = &p->anim;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->riseTimer = 60;
        p->frame = 0;
        p->entered = 0;
    }
    if (p->ended) {
        s32 value = RandomByte();
        if (value <= 63) {
            register const u16 *randomTable asm("r2") = gRandomTable;
            void *resource = pool->resource;
            asm("" : : "r"(randomTable));
            Anim_SetSequence(sequence, resource, 21, 0, 0);
        } else if (value <= 77)
            Anim_SetSequence(sequence, pool->resource, 22, 0, 0);
        else
            Anim_SetSequence(sequence, pool->resource, 20, 0, 0);
        p->ended = 0;
    }
    if (!p->delay) {
        struct Vec delta;
        s32 turn;
        Vec_Subtract(&delta, &sprite->position, p->target);
        turn = Math_Atan2(delta.x, delta.z) - p->heading;
        if (turn > 128)
            turn -= 256;
        else if (turn < (-128)) {
            register s32 wrap asm("r2") = 128;
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
    } else
        p->delay--;
    Polar(&p->velocity, p->heading, p->speed);
    if ((!Mod(p->frame, 10)) && (p->speed < p->maxSpeed))
        p->speed++;
    sprite->position.x += p->velocity.x;
    if (p->riseTimer) {
        s32 targetY = p->target->y;
        s32 currentY = sprite->position.y;
        currentY -= 200;
        sprite->position.y += Div(targetY - currentY, p->riseTimer);
        p->riseTimer--;
    }
    sprite->position.z += p->velocity.z;
    {
        u32 ended = Step(sprite, sequence);
        register u32 offset asm("r1") = (u32) & ((struct Effect *)0)->ended;
        asm("" : "+r"(offset));
        {
            register u8 *field asm("r0") = (u8 *)p + offset;
            *field = ended;
        }
    }
    LinkedEffect_UpdateColliders(p);
    {
        register u32 offset asm("r2") = (u32) & ((struct Effect *)0)->frame;
        register u16 *frame asm("r1");
        asm("" : "+r"(offset));
        frame = (u16 *)((u8 *)p + offset);
        (*frame)++;
    }
    if (p->lifetime)
        p->lifetime--;
    else {
        p->state = 3;
        p->entered = 1;
    }
}
