// MATCHING_CONCESSION: register-pinning
// scope: write LinkedEffect_FollowTarget here
#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "player_cue_sequence.h"
// confidence=low: steers and accelerates a linked effect toward a target until expiry.
#define LinkedEffect_FollowTarget sub_0812ACB0
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
            const u16 *randomTable = gRandomTable;
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
    p->ended = PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite,
                                           (struct DisplaySequenceState *)sequence);
    LinkedEffect_UpdateColliders(p);
    p->frame++;
    if (p->lifetime)
        p->lifetime--;
    else {
        p->state = 3;
        p->entered = 1;
    }
}
