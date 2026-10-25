#include "global.h"

#ifdef NONMATCHING
// fuzzy: 86.5%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: vector and random-source lifetimes, stack addressing and register allocation.
struct Vec {
    s16 x, y, z, pad;
};
struct PackedPosition {
    u32 xy, z;
};
struct Actor {
    u8 pad0[0x54];
    struct Vec position;
    u8 pad5c[0x34];
    u32 timer;
    u8 pad94[0x16];
    u8 animation, padAB[4], ready, active;
    u8 padB1[0x1b];
    u8 angle;
    u8 padCD[0xb3];
    s32 sequence, complete;
    u8 pad188[0xb44];
    struct PackedPosition points[9];
    u8 padD14[0x146];
    u16 repeat;
    struct Vec destination, center;
    u16 duration;
    u8 padE6e[6];
    s16 hitTimer;
    u8 padE76[0x14];
    s16 previousAngle, ended;
    u8 padE8e[0x50];
    s16 waitTimer, particleTimer;
    u8 padEe2[4];
    s16 angleLimit;
};
extern u8 *gUnk_02000580;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
struct Vec *Player_GetEffectivePosition(s32);
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void CpuSet(const void *, void *, u32);
void Player_BeginInterpolatedMovement(void *, void *, s32, s32, s32, s32, s32);
void Player_ApplyInterpolatedMovement(void *, void *, s32, s32, s32, s32, s32, s32);
void Player_EndState25(void *);
void SlotMotion_Initialize(void *);
void Ratatosk_ConfigureParticle(void *, s32, s32, s32, s32, s32);
void SceneParticles_SpawnAtPosition(void *, void *);
#define HALF(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define BYTE(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define WORD(p, o) (*(u32 *)((u8 *)(p) + (o)))
static inline void setAnimation(struct Actor *p, u32 animation)
{
    p->animation = animation;
}
static inline u8 consume(struct Actor *p)
{
    if (p->active) {
        p->active = 0;
        p->ready = 0;
        return 1;
    }
    return 0;
}
static inline u32 randomB(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct Vec *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
static inline void difference(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline u32 flag(void *p, u32 mask)
{
    return WORD(p, 0x20) & mask;
}
// confidence=low: draws the player into a midpoint orbit before releasing movement.
#define Ratatosk_UpdatePlayerOrbitAttack sub_081F7FD0
inline static void sum_081F7FD0(struct Vec *d, struct Vec *a, struct Vec *b)
{
    {
        typeof(a->x) v2 = a->x;
        d->x = v2 + b->x;
    }
    {
        typeof(a->y) v1 = a->y;
        d->y = v1 + b->y;
    }
    d->z = a->z + b->z;
}

void Ratatosk_UpdatePlayerOrbitAttack(struct Actor *p)
{
    struct Actor *q = p;
    struct {
        u32 zero;
        u32 config[6];
        struct Vec offset;
        struct Vec delta;
        struct Vec position;
    } scratch;
    if (consume(q)) {
        setAnimation(q, 9);
        p->sequence = 0;
        p->repeat = 0;
        p->waitTimer = 0;
        p->duration = 64;
        Sound_PlaySong(0x558);
    }
    switch (p->sequence) {
        case 0:
            if (q->ready) {
                if (p->hitTimer > 0) {
                    struct Vec *playerPosition;
                    u8 *player;
                    p->sequence = 1;
                    playerPosition = Player_GetEffectivePosition(0);
                    difference(&p->center, playerPosition, &q->position);
                    p->center.x = q->position.x + Div(p->center.x, 2);
                    p->center.y = q->position.y;
                    p->center.z = q->position.z + Div(p->center.z, 2);
                    p->duration = 128;
                    polar(&scratch.offset, ((((((u8)randomB()) + 0x60) >> 6) & 3) * 64) + 64, 512);
                    sum_081F7FD0(&p->destination, &scratch.offset, &p->center);
                    player = gUnk_02000580;
                    if (flag(player, 0x40000) != 1) {
                        Player_BeginInterpolatedMovement(player, &p->center, p->duration, 0, 476, 0,
                                                         0);
                        p->previousAngle = *((u8 *)(((u8 *)gUnk_02000580) + 0x4f4));
                        p->ended = 0;
                        Sound_PlaySong(0x569);
                    }
                    setAnimation(q, 5);
                } else if ((++p->waitTimer) > 2) {
                    p->duration = 64;
                    p->complete = 1;
                } else {
                    s32 index = Mod(randomB(), 9);
                    *((struct PackedPosition *)(&p->destination)) = p->points[index];
                    setAnimation(q, 4);
                    p->sequence = 3;
                }
            }
            break;

        case 1: {
            s32 prev = p->previousAngle;
            u8 *player = gUnk_02000580;
            s32 amount = *((u8 *)(((u8 *)player) + 0x4f4));
            if (amount < prev)
                amount -= prev - 256;
            else
                amount -= prev;
            if (amount > p->angleLimit) {
                if (p->ended)
                    goto finish;
                Player_EndState25(gUnk_02000580);
                p->ended = 1;
                Sound_StopSong(0x569);
            }
            if (!p->ended) {
                if ((((u8)randomB()) < 8) && (!p->particleTimer))
                    p->particleTimer = 10;
                if (p->particleTimer > 0) {
                    s32 step = Mod(q->timer, 32);
                    difference(&scratch.delta, &q->position, Player_GetEffectivePosition(0));
                    if (!step) {
                        scratch.zero = 0;
                        CpuSet(&scratch.zero, &scratch.delta, 0x05000002);
                    } else {
                        scratch.delta.x = Div(scratch.delta.x * step, 32);
                        scratch.delta.y = 196;
                        scratch.delta.z = Div(scratch.delta.z * step, 32);
                    }
                    sum_081F7FD0(&scratch.position, &scratch.delta, Player_GetEffectivePosition(0));
                    scratch.position.y += 128;
                    SlotMotion_Initialize(scratch.config);
                    Ratatosk_ConfigureParticle(scratch.config, 10, 0, 0, 0, 0);
                    SceneParticles_SpawnAtPosition(scratch.config, &scratch.position);
                    p->particleTimer--;
                }
            }
        finish:
            if (q->ready) {
                setAnimation(q, 7);
                p->waitTimer = 0;
                p->sequence = 2;
            }

            break;
        }

        case 2:
            if ((p->waitTimer == 13) && (!p->ended)) {
                u8 *player = gUnk_02000580;
                if (flag(player, 0x40000) != 1)
                    Player_ApplyInterpolatedMovement(player, &p->center, 30, q->angle, 0, 42, 30,
                                                     0);
                Sound_StopSong(0x569);
            }
            if (q->ready) {
                p->duration = 64;
                p->complete = 1;
            }
            p->waitTimer++;
            break;

        case 3:
            if (q->ready) {
                setAnimation(q, 9);
                p->sequence = 0;
                p->repeat = 0;
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F7FD0);
#endif
