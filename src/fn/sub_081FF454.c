// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// fix.py best after the inlined-insn fix
struct HomingVec {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct LocalVec {
    s16 x, y, z;
} __attribute__((aligned(4)));
struct HomingParams {
    u8 unknown0[2];
    u16 delay;
    u8 unknown4[2];
    s8 angle;
    u8 variant;
    s32 speed;
    void *behavior;
    s16 attack, element, attackFlags, body;
    s16 shotA, shotB, shotC;
    u8 unknown1E[2];
};
struct HomingCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4c];
};
struct HomingSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct HomingSprite {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xa];
    struct HomingVec position;
    u8 unknown24[8];
};
struct HomingSlot {
    u32 lifetime;
    u8 unknown04[8];
    struct HomingVec velocity;
    struct HomingVec origin;
    struct HomingVec target;
    struct HomingCollider secondaryCollider;
    struct HomingCollider primaryCollider;
    u8 animData[0x1c];
    void *resource;
    struct HomingParams params;
    struct HomingSprite sprite;
    u32 unknown138;
    s32 state;
    struct HomingSequence sequence;
    u8 unknown150[0xc];
    s16 flashTimer;
    s16 blinkTimer;
};
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);
struct HomingVec *Player_GetEffectivePosition(s32);
s32 Math_Atan2(s32, s32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void SunDiscPool_SpawnDisc(void *, void *, s32, s32, s32, s32, s32, s32, s32);
void BouncingObject_Spawn(void *, void *, void *, s32, s32, s32, s32);
u16 Sqrt(u32);
s32 Div(s32, s32);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

static inline void SpawnTrail(struct HomingSlot *p)
{
    u32 seed, t;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    seed = gRandomIndex;
    t = *(u8 *)&gRandomTable[seed];
    SpriteParticle_CreateDefaultVariant4384(&p->origin, 0x28, 0x20, 0xc, 4, 0x18, t, 0x20, 0x20, 3,
                                            2, 0x200);
}

static inline void Face(struct HomingSlot *p, struct HomingParams *params, u32 sequence, u32 one)
{
    u8 direction;
    u8 flips[2];
    u8 *first;
    u8 *second;
    u8 *saved;
    u32 check;
    direction = (((u16)params->angle + 0x60) >> 6) & 3;
    first = &flips[0];
    second = &flips[1];
    check = (u8)(direction - 1);
    saved = second;
    if (check <= 1)
        *first = one;
    else
        *first = 0;
    if (direction > 1)
        *second = 1;
    else
        *second = 0;
    Anim_SetSequence(&p->sequence, p->resource, (u16)sequence, flips[0], *saved);
}

static inline void FaceFinal(struct HomingSlot *p, struct HomingParams *params, u32 sequence,
                             u32 one)
{
    u8 direction;
    u8 flips[2];
    u8 *first;
    u8 *second;
    u8 *saved;
    u32 check;
    direction = (((u16)params->angle + 0x60) >> 6) & 3;
    first = &flips[0];
    second = &flips[1];
    check = (u8)(direction - 1);
    saved = second;
    if (check <= 1)
        *first = one;
    else
        *first = 0;
    if (direction > 1)
        *second = 1;
    else
        *second = 0;
    asm("" : : "r"(saved) : "r5");
    Anim_SetSequence(&p->sequence, p->resource, (u16)sequence, flips[0], *saved);
}

static inline void Aim(struct HomingSlot *p, struct HomingParams *params)
{
    struct HomingVec *player = Player_GetEffectivePosition(0);
    struct HomingVec *position = &p->sprite.position;
    params->angle = Math_Atan2(player->x - position->x, player->z - position->z);
}

static inline void SetRiseHeight(struct HomingSlot *p, u16 height)
{
    register u32 offset = 0x12a;
    {
        register s16 *dest asm("r2") = (void *)((u8 *)p + offset);
        *dest = height;
    }
}
inline static void SpawnTrail_UpdateDefaultState(struct HomingSlot *p)
{
    u32 seed;
    u32 t;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    seed = gRandomIndex;
    t = *((u8 *)(&(*((typeof(gRandomTable) *)0x0203B400))[seed]));
    SpriteParticle_CreateDefaultVariant4384(&p->origin, 0x28, 0x20, 0xc, 4, 0x18, t, 0x20, 0x20, 3,
                                            2, 0x200);
}

u32 HomingProjectile_UpdateDefaultState(struct HomingSlot *p, s32 slot)
{
    struct HomingParams *params = &p->params;
    s32 *state = &p->state;
    u32 sequence;
    switch (*state) {
        case 0: {
            register u32 one;
            u32 time = p->lifetime;
            one = 1;
            asm("" : "+r"(one));
            if (time & one)
                SpawnTrail_UpdateDefaultState(p);
            p->sprite.flags |= 0x400;
            p->sprite.position.y = p->target.y - ((30 - p->lifetime) * 2);
            if (p->lifetime > 30) {
                *state = one;
                p->sprite.flags &= ~0x400;
                Aim(p, params);
                sequence = (params->variant == 0) ? (7) : (0);
                Face(p, params, sequence, one);
            }
            break;
        }

        case 1: {
            register u16 *delay = &p->params.delay;
            s32 limit = (slot * 30) + 30;
            asm("" : "+r"(delay));
            if (p->lifetime > (limit + (*delay))) {
                Aim(p, params);
                sequence = (params->variant == 0) ? (10) : (6);
                p->sequence.flags |= 8;
                Face(p, params, sequence, 1);
                p->state = 2;
            }
            break;
        }

        case 2:
            if (params->variant == 0) {
                if (p->sequence.index != 5)
                    break;
                if (p->sequence.elapsed != 0)
                    break;
                {
                    struct LocalVec from = {p->sprite.position.x, p->sprite.position.y + 0xc4,
                                            p->sprite.position.z};
                    struct LocalVec to;
                    struct LocalVec *player = (struct LocalVec *)Player_GetEffectivePosition(0);
                    u16 *q = (u16 *)(&to);
                    to = *player;
                    q[1] += 0x60;
                    SunDiscPool_SpawnDisc(&from, &to, p->params.shotA, p->params.shotB, 2,
                                          p->params.shotC, 0x3c, 0x1e, -20);
                }
            } else {
                u32 elapsed;
                if (p->sequence.index != 4)
                    break;
                elapsed = p->sequence.elapsed;
                if (elapsed != 0)
                    break;
                {
                    struct LocalVec from = {p->sprite.position.x, p->sprite.position.y + 0x100,
                                            p->sprite.position.z};
                    struct LocalVec to;
                    struct LocalVec delta;
                    struct LocalVec *player = (struct LocalVec *)Player_GetEffectivePosition(0);
                    struct LocalVec *t = &to;
                    struct LocalVec *d;
                    struct LocalVec *f;
                    u16 length;
                    s32 speed;
                    *t = *player;
                    d = &delta;
                    f = &from;
                    d->x = t->x - f->x;
                    d->y = t->y - f->y;
                    d->z = t->z - f->z;
                    speed = 30;
                    length = Sqrt(((d->x * d->x) + (d->y * d->y)) + (d->z * d->z));
                    if (length) {
                        d->x = Div(d->x * speed, length);
                        d->y = Div(d->y * speed, length);
                        d->z = Div(d->z * speed, length);
                    }
                    BouncingObject_Spawn(f, d, t, p->params.shotA, p->params.shotB, 0x50, elapsed);
                }
            }
            sequence = (params->variant == 0) ? (7) : (0);
            FaceFinal(p, params, sequence, 1);
            p->state = 3;
            {
                struct HomingCollider *collider = &p->primaryCollider;
                collider->flags |= 4;
            }
            p->blinkTimer = 0;
            break;

        case 3:
            if (p->lifetime & 1)
                SpawnTrail_UpdateDefaultState(p);
            {
                s32 limit = slot * 30;
                u32 elapsed = p->lifetime - 200;
                SetRiseHeight(p, p->target.y - ((30 - (limit - elapsed)) * 2));
            }
            {
                register u32 spriteOffset;
                register u32 timerOffset;
                register struct HomingSprite *sprite;
                register s16 *timer;
                spriteOffset = 0x10c;
                sprite = (void *)(((u8 *)p) + spriteOffset);
                timerOffset = 0x15e;
                timer = (void *)(((u8 *)p) + timerOffset);
                {
                    s32 timerValue = *timer;
                    if (Sprite_BlinkUntilExpired(sprite, timerValue, 0x2d, 0x5a))
                        return 1;
                    (*timer)++;
                }
            }
            break;
    }

    return 0;
}
