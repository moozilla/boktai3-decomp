// MATCHING_CONCESSION: register-pinning
#include "homing_projectile.h"
#include "engine_globals.h"
#include "ram.h"
struct LocalVec {
    s16 x, y, z;
} __attribute__((aligned(4)));
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);
struct ContactPosition *Player_GetEffectivePosition(s32);
s32 Math_Atan2(s32, s32);
u32 Anim_SetSequence(struct DisplaySequenceState *, u8 *, u16, u8, u8);
void SunDiscPool_SpawnDisc(void *, void *, s32, s32, s32, s32, u32, s32, s32);
void BouncingObject_Spawn(void *, void *, void *, s32, s32, s32, s32);
u16 Sqrt(u32);
s32 Div(s32, s32);
s32 Sprite_BlinkUntilExpired(u32 *, s32, s32, s32);

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
    Anim_SetSequence(&p->sequence, p->graphics.resource, (u16)sequence, flips[0], *saved);
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
    Anim_SetSequence(&p->sequence, p->graphics.resource, (u16)sequence, flips[0], *saved);
}

static inline void Aim(struct HomingSlot *p, struct HomingParams *params)
{
    struct ContactPosition *player = Player_GetEffectivePosition(0);
    struct ContactPosition *position = &p->sprite.position;
    params->angle = Math_Atan2(player->x - position->x, player->z - position->z);
}

static inline void SetRiseHeight(struct HomingSlot *p, u16 height)
{
    register s16 *dest asm("r2") = &p->sprite.position.y;
    *dest = height;
}
static inline void SpawnTrail(struct HomingSlot *p)
{
    u32 seed;
    u32 t;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    seed = gRandomIndex;
    t = *((u8 *)(RANDOM_TABLE_ADDR + sizeof(*gRandomTable) * seed));
    SpriteParticle_CreateDefaultVariant4384(&p->origin, 0x28, 0x20, 0xc, 4, 0x18, t, 0x20, 0x20, 3,
                                            2, 0x200);
}

s32 HomingProjectile_UpdateDefaultState(struct HomingSlot *p, s32 slot)
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
                SpawnTrail(p);
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
                SpawnTrail(p);
            {
                s32 limit = slot * 30;
                u32 elapsed = p->lifetime - 200;
                SetRiseHeight(p, p->target.y - ((30 - (limit - elapsed)) * 2));
            }
            if ((u8)Sprite_BlinkUntilExpired(&p->sprite.flags, p->blinkTimer, 0x2d, 0x5a))
                return 1;
            p->blinkTimer++;
            break;
    }

    return 0;
}
