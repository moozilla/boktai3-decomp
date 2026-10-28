#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 93.3%

// Remaining: polar address reuse, signed RNG/direction loads, collider loop allocation and phase-tail scheduling.

// confidence=low: axe attack behavior inferred from orbit and trail state updates.

// Branch layout, address reuse and register allocation still differ.
// confidence=low: the axe and its auxiliary sprite orbit a shared moving center.
#define H(p, off) (*(u16 *)((u8 *)(p) + (off)))
#define S(p, off) (*(s16 *)((u8 *)(p) + (off)))
#define B(p, off) (*((u8 *)(p) + (off)))
#define W(p, off) (*(u32 *)((u8 *)(p) + (off)))
#define P(p, off) ((u8 *)(p) + (off))
struct Vec {
    s16 x, y, z, pad;
};
struct Collider {
    u8 pad[6];
    u16 flags;
    u8 rest[0x4C];
};
struct AxeContext {
    u8 pad0[0xA38];
    u32 spriteFlags;
    u8 padA3C[0x2];
    u8 spriteAngle;
    u8 padA3F[0x15];
    u16 spriteX;
    u8 padA56[0x2];
    u16 spriteZ;
    u8 padA5A[0x36];
    struct Collider attack[3];
    struct Collider body[3];
    struct Vec origin;
    u8 padC90[0x8];
    struct Vec center;
    s16 radius;
    u8 padCA2[0x2];
    u8 speed;
    s8 direction;
    u8 heading;
    u8 padCA7[0x2];
    u8 angle;
    u8 padCAA[0x1];
    u8 flags;
    u8 padCAC[0x204];
    u8 timer;
};
struct ColliderFlags {
    u8 pad[6];
    u16 flags;
};
struct AxeActor {
    u8 pad0[0x4];
    u8 *sprite;
    u8 pad8[0x54];
    struct Vec position;
    u8 pad64[0xA0];
    u32 word104;
    u8 pad108[0xC];
    u32 flags114;
    u8 pad118[0x1C];
    struct ColliderFlags bodyCollider;
    u8 pad13C[0x4C];
    struct ColliderFlags attackCollider;
    u8 pad190[0x8C];
    void *callback;
    u8 pad220[0x90];
    u8 state;
    u8 pad2B1[0x1];
    u8 entered;
    u8 pad2B3[0x2];
    u8 phase;
    u8 pad2B6[0x12];
    u16 timer;
    u8 pad2CA[0x106];
    struct AxeContext *context;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u8 *gPlayers;
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void Axe_UpdateAttackColliderPositions(void *);
void ActorPhase_UpdateAcceleratingTrail(void);
void ActorPhase_UpdateReverseFlicker(void);
static inline u8 TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void Difference(struct Vec *v, struct Vec *a, struct Vec *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline void Add(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline s32 Scale(s32 value, s32 bits)
{
    if (value >= 0)
        return value >> bits;
    return -(-value >> bits);
}
static inline void Clear(u32 mask, u32 *flags)
{
    *flags &= mask;
}
static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}
static inline void ChangePhase(struct AxeActor *p, void *callback, u32 phase)
{
    p->entered = 1;
    p->state = 0;
    p->phase = phase;
    p->callback = callback;
    p->timer = 0;
    ClearFlags(&p->flags114, ~1);
    p->word104 = 0;
}
void Axe_UpdateOrbitAttack(struct AxeActor *p, s32 frame)
{
    struct AxeContext *data = p->context;
    struct Vec delta;
    struct Vec *position;
    u8 entered;
    s32 difference, absolute;
    entered = TakeEntry(&p->entered);
    position = &p->position;
    if (entered) {
        data->flags |= 2;
        data->angle = 0;
        data->radius = 128;
        data->speed = 4;
        data->direction = 1;
        data->center = data->origin;
        Difference(&delta, position, &data->center);
        delta.x = Scale(delta.x, 1);
        delta.z = Scale(delta.z, 1);
        Add(&data->center, &data->center, &delta);
    }
    position->x =
        data->center.x + Scale(gUnk_086149C4[(data->angle + 64) & 255] * data->radius, 12);
    p->position.z = data->center.z + Scale(gUnk_086149C4[data->angle] * data->radius, 12);
    data->spriteX =
        data->center.x +
        Scale(gUnk_086149C4[(((data->angle + 128) & 255) + 64) & 255] * data->radius, 12);
    data->spriteZ =
        data->center.z + Scale(gUnk_086149C4[(data->angle + 128) & 255] * data->radius, 12);
    if (frame < 60) {
        data->speed += 2;
        data->timer = 0;
    } else if (frame < 180) {
        if (Mod(frame, 4) == 0) {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            if ((s32) * (const u8 *)&gUnk_0203B400[gRandomIndex] <= 19) {
                {
                    s32 direction = data->direction;
                    data->direction = -direction;
                }
                Sound_PlaySong(0x169);
            }
        }
        Difference(&delta, (void *)P(gPlayers, 0x30), &data->center);
        data->heading = Math_Atan2(delta.x, delta.z);
        data->timer = 0;
    } else {
        if (!(data->flags & 4)) {
            if (data->radius < 512) {
                data->radius += 8;
                data->speed -= 4;
                if (data->speed <= 15)
                    data->speed = 16;
            } else {
                data->radius = 512;
                data->speed = 16;
                data->flags |= 4;
            }
        }
        Difference(&delta, (void *)P(gPlayers, 0x30), &data->center);
        difference = Math_Atan2(delta.x, delta.z) - data->heading;
        if (difference > 128)
            difference -= 256;
        else if (difference < -128)
            difference += 256;
        absolute = difference;
        if (difference < 0)
            absolute = -difference;
        if (absolute > 20) {
            if (difference > 0)
                data->heading += 4;
            else
                data->heading -= 4;
        }
        data->center.x += Scale(gUnk_086149C4[(data->heading + 64) & 255] * 2, 12);
        data->center.z += Scale(gUnk_086149C4[data->heading] * 2, 12);
        if (data->flags & 4) {
            u8 *sprite = p->sprite;
            u32 previous;
            W(sprite, 0) |= 2;
            data->spriteFlags |= 2;
            previous = B(sprite, 6);
            B(sprite, 6) = previous - 6;
            data->spriteAngle += 6;
            if ((s32)previous < B(p->sprite, 6))
                Sound_PlaySong(0x169);
            if (data->flags & 4) {
                if (frame < 480)
                    data->radius--;
                else if (frame < 580)
                    data->radius += 6;
                else if (frame < 660)
                    data->radius -= 4;
                else if (frame < 740)
                    data->radius += 4;
                else if (frame > 819)
                    data->radius -= 6;
            }
        }
    }
    data->angle += Scale(data->speed * (s8)data->direction, 4);
    Axe_UpdateAttackColliderPositions(p);
    if (data->flags & 1) {
        ChangePhase(p, ActorPhase_UpdateAcceleratingTrail, 0x2E);
    } else if (frame > 1000) {
        s32 i;
        {
            struct ColliderFlags *c = &p->attackCollider;
            c->flags |= 4;
        }
        {
            struct ColliderFlags *c = &p->bodyCollider;
            c->flags |= 4;
        }
        for (i = 0; i < 3; i++) {
            data->attack[i].flags |= 4;
            data->body[i].flags |= 4;
        }
        ChangePhase(p, ActorPhase_UpdateReverseFlicker, 0x2F);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Axe_UpdateOrbitAttack);
#endif
