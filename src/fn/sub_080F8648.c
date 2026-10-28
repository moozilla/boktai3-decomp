#include "global.h"
#include "player.h"

// confidence=low: axe attack behavior inferred from orbit and trail state updates.

// Two instructions differ: the global address load is scheduled after the stack-vector address.
// confidence=low: sends the axe trail toward the player, then returns it to the actor.
struct Vec {
    s16 x, y, z, pad;
};
struct AxeTrail {
    u8 unknown0[0xA24];
    struct Vec position, velocity;
    u8 angle, heading, spinRate;
    u8 unknownA37[0x405];
    u8 angleDecay, speed;
};
struct AxeActor {
    u8 unknown0[4];
    u8 *sprite;
    u8 unknown8[0x54];
    struct Vec position;
    u8 unknown64[0x85];
    u8 facing;
    u8 unknownEA[0x1A];
    u32 word104;
    u8 unknown108[0xC];
    u32 flags114;
    u8 unknown118[0x104];
    void *callback;
    u8 unknown220[0x90];
    u8 state, unknown2B1, entered, unknown2B3[2], phase;
    u8 unknown2B6[0x12];
    u16 timer;
    u8 unknown2CA[0x106];
    struct AxeTrail *trail;
};
extern s16 gUnk_086149C4[];
void Axe_UpdateTrailColliders(void *);
void ActorPhase_EmitTrailSprite(void *, struct Vec *, u32);
void Sound_PlaySong(u32);
s32 Math_Atan2(s32, s32);
void SceneActorControl_SettleRotation(void);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void Velocity(struct Vec *out, u8 angle, s32 speed)
{
    out->x = Scale(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Scale(speed * gUnk_086149C4[angle]);
}
static inline void Difference(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline void Advance(struct Vec *a, struct Vec *b)
{
    a->x = b->x + a->x;
    a->y = b->y + a->y;
    a->z = b->z + a->z;
}
static inline s32 AngleDifference(s32 a, s32 b)
{
    s32 delta = a - b;
    if (delta > 128)
        delta -= 256;
    else if (delta < -128)
        delta += 256;
    return delta;
}
static inline s32 LengthSquared(struct Vec *v)
{
    return v->x * v->x + v->z * v->z;
}
static inline s32 Absolute(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
static inline void ClearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}
void Axe_UpdateReturningTrail(struct AxeActor *p, s32 frame)
{
    struct AxeTrail *trail = p->trail;
    u32 previous;
    if (Take(&p->entered)) {
        *(u32 *)p->sprite |= 2;
        Velocity(&trail->velocity, p->facing, 12);
        trail->heading = p->facing;
        trail->position = p->position;
        trail->angle = p->sprite[6];
        trail->spinRate = 1;
    }
    previous = p->sprite[6];
    p->sprite[6] = previous - trail->spinRate;
    if ((frame & 1) && trail->spinRate <= 11)
        trail->spinRate++;
    Axe_UpdateTrailColliders(p);
    if ((s32)previous < p->sprite[6])
        Sound_PlaySong(0x169);
    if (frame <= 119) {
        struct Vec delta;
        s32 difference;
        ActorPhase_EmitTrailSprite(p, &trail->position, trail->angle);
        Advance(&trail->position, &trail->velocity);
        trail->angle -= trail->angleDecay;
        {
            u8 **gp = &gPlayers;
            struct Vec *d;
            d = &delta;
            Difference(d, (void *)(*gp + 0x30), &trail->position);
        }
        difference = AngleDifference(Math_Atan2(delta.x, delta.z), trail->heading);
        if (Absolute(difference) > 32)
            trail->heading += 4;
        Velocity(&trail->velocity, trail->heading, trail->speed);
    } else {
        struct Vec delta;
        s32 difference;
        ActorPhase_EmitTrailSprite(p, &trail->position, trail->angle);
        Advance(&trail->position, &trail->velocity);
        trail->angle -= 6;
        Difference(&delta, &p->position, &trail->position);
        difference = AngleDifference(Math_Atan2(delta.x, delta.z), trail->heading);
        if (Absolute(difference) > 12)
            trail->heading += 6;
        Velocity(&trail->velocity, trail->heading, 8);
        if (LengthSquared(&delta) <= 1023 || frame > 600) {
            trail->position = p->position;
            {
                void *callback = SceneActorControl_SettleRotation;
                u32 phase = 0x29;
                p->entered = 1;
                p->state = 0;
                p->phase = phase;
                p->callback = callback;
                p->timer = 0;
                ClearFlags(&p->flags114, 1);
                p->word104 = 0;
            }
        }
    }
}
