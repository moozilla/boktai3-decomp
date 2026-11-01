// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "player_cue_sequence.h"
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
static inline const u16 *RandomEntry(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return (const u16 *)((index << 1) + (u32)gRandomTable);
}
static inline s32 Truncate(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
struct UnsignedVector {
    u16 x, y, z, pad;
};
void LinkedEffect_SetVariant(void *, u32);
// confidence=low: expands the linked effect while preparing an elevated travel target.
void LinkedEffect_BeginRisingTravel(struct Pool *pool, struct Effect *p)
{
    if (p->entered) {
        u32 heading;
        u32 sample, value;
        const u16 *random;
        register u32 distance asm("r4");
        struct UnsignedVector *velocity;
        p->sprite.flags = (p->sprite.flags & ~1) | 2;
        p->ended = 0;
        heading = p->heading;
        random = RandomEntry();
        sample = 255;
        value = (u8)*random;
        /* Keep the random sample separate from the travel distance. */
        asm("" : "+r"(value));
        velocity = (void *)&p->velocity;
        distance = value + 64;
        velocity->x = Truncate(distance * gUnk_086149C4[(heading + 64) & sample]);
        velocity->y = 0;
        velocity->z = Truncate(distance * gUnk_086149C4[heading]);
        {
            struct Vec *destination = (void *)&p->velocity;
            struct Vec *source = (void *)&p->sprite.position;
            destination->x = source->x + destination->x;
            destination->y = source->y + destination->y;
            destination->z = source->z + destination->z;
        }
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
    p->ended = PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->sprite,
                                           (struct DisplaySequenceState *)&p->anim);
    if (p->ended && !p->riseTimer) {
        p->state = 2;
        p->sprite.flags &= ~2;
        p->entered = 1;
    } else
        p->frame++;
}
