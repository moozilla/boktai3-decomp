#include "global.h"

#define PlayerClone_UpdateBehavior sub_082003BC

extern u32 gUnk_03005308, gUnk_03005304;
extern u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_020005F8;

struct CloneVec {
    s16 x, y, z, pad;
};

void ScaledShadow_Enable(void *);
void ScaledShadow_Disable(void *);
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_SelectPaletteBank(void *, u32);
void ActorDebris_Show(u32);
void PlayerEffect_SetPaletteSource(u32, u32, u32);
struct CloneVec *Player_GetEffectivePosition(s32);
struct CloneVec *Ratatosk_GetPosition(void);
void SlotMotion_Initialize(u8 *);
void Ratatosk_ConfigureParticle(u8 *, u32, u32, u32, u32, u32);
int SceneParticles_SpawnAtPosition(u8 *, void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

static inline void Vec_FromAngle(struct CloneVec *v, s32 angle, s32 speed)
{
    v->x = Scale(gUnk_086149C4[(angle + 0x40) & 0xff] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 0xff] * speed);
}

static inline void Vec_Sum(struct CloneVec *d, struct CloneVec *a, struct CloneVec *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}

static inline void Vec_Diff(struct CloneVec *d, struct CloneVec *a, struct CloneVec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

static inline s32 RandomByte(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return *(u8 *)(gUnk_03005308 * 2 + RANDOM_TABLE_ADDR);
}

static inline u16 RandomAngle(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3ff;
    return gUnk_086141BC[gUnk_03005304];
}

static inline void Flags_Set(u8 *p, u32 m)
{
    *(u32 *)(p + 8) |= m;
}

static inline void Flags_Clear(u8 *p, u32 m)
{
    *(u32 *)(p + 8) &= ~m;
}

static inline void Bits_Set(u32 m, u8 *p)
{
    *(u32 *)p |= m;
}

#define W(o)         (*(u32 *)(s + (o)))
#define H(o)         (*(s16 *)(s + (o)))
#define UH(o)        (*(u16 *)(s + (o)))
#define B(o)         (*(u8 *)(s + (o)))
#define CTX_DIR(c)   (*(s8 *)((c) + 6))
#define CTX_SPEED(c) (*(s32 *)((c) + 8))

#ifdef NONMATCHING
// fuzzy: 81.5%
// Remaining differences: register allocation in case 0 (speed lands in r8, the
// Vec_Sum source pointer is computed before Div), constant reuse for s + 0x278
// in case 3, and scheduling of the zero/sb copies in case 3.
s32 PlayerClone_UpdateBehavior(u8 *s, s32 i)
{
    u8 *ctx = s + 0x198;
    vu32 zero;
    u8 motion[0x18];
    struct CloneVec v;
    struct CloneVec d;
    struct CloneVec e;

    switch (W(0x308)) {
        case 0: {
            u8 *c1 = s + 0x80;
            s32 dir;
            s32 speed;
            struct CloneVec *src;
            *(s16 *)(c1 + 6) |= 4;
            ScaledShadow_Enable(s + 0x298);
            Flags_Set(s + 0x1b8, 1);
            src = (struct CloneVec *)(s + 0x24);
            dir = CTX_DIR(ctx);
            speed = Div(W(0) << 9, 30);
            Vec_FromAngle(&v, dir + 0x80, speed);
            {
                struct CloneVec *dst = (struct CloneVec *)(s + 0x1d8);
                Vec_Sum(dst, &v, src);
            }
            if (W(0) > 30) {
                Flags_Clear(s + 0x1b8, 1);
                *(s16 *)(c1 + 6) &= ~4;
                W(0x308) = 1;
                W(0x304) &= ~1;
                {
                    u8 *dst = s + 0xc;
                    zero = 0;
                    CpuSet((void *)&zero, dst, 0x05000002);
                }
            }
            break;
        }
        case 1:
            if (W(0) & 1)
                SpriteParticle_CreateDefaultVariant4384(s + 0x14, 0x28, 0x20, 0xc, 4, 0x18,
                                                        RandomByte(), 0x20, 0x20, 3, 2, 0x200);
            Flags_Set(s + 0x1b8, 0x400);
            H(0x1da) = H(0x1e) - (60 - W(0)) * 2;
            if (W(0) > 60) {
                W(0x308) = 2;
                Flags_Clear(s + 0x1b8, 0x400);
                ScaledShadow_Disable(s + 0x298);
            }
            break;
        case 2:
            if (W(0) > 0x120)
                W(0x308) = 3;
            break;
        case 3:
            if (Mod(W(0), 120) == 0) {
                ctx[6] = RandomAngle();
                Bits_Set(1, s + 0x304);
                W(0x31c) = 5;
                Anim_LoadKeyframe(s + 0x1b8, s + 0x278, 5, 1);
            }
            if (W(0) > i * 30 + 0x294) {
                u32 z;
                *(struct CloneVec *)(s + 0x1c) = *(struct CloneVec *)(s + 0x1d8);
                z = 0;
                UH(0x14) = UH(0x1d8);
                UH(0x16) = UH(0x1da) + 0x80;
                UH(0x18) = UH(0x1dc);
                W(0x308) = 5;
                CTX_SPEED(ctx) = z;
                Bits_Set(1, s + 0x304);
                {
                    u8 *c1 = s + 0x80;
                    *(s16 *)(c1 + 6) |= 4;
                }
                UH(0x340) = z;
                W(0x31c) = z;
                Anim_LoadKeyframe(s + 0x1b8, s + 0x278, 0, 1);
            } else {
                struct CloneVec *p = Player_GetEffectivePosition(0);
                struct CloneVec *pos = (struct CloneVec *)(s + 0x1d8);
                s32 dx = p->x - pos->x;
                s32 dz = p->z - pos->z;
                if (dx * dx + dz * dz <= 0xffff) {
                    u8 *anim, *res;
                    u8 *t;
                    s32 z = 0;
                    H(0x332) = z;
                    CTX_SPEED(ctx) = z;
                    Bits_Set(1, s + 0x304);
                    p = Player_GetEffectivePosition(0);
                    ctx[6] = Math_Atan2(p->x - pos->x, p->z - pos->z);
                    W(0x31c) = 0x54;
                    anim = s + 0x1b8;
                    res = s + 0x278;
                    Anim_LoadKeyframe(anim, res, 0x54, 2);
                    Anim_SetKeyframes(anim, res, (u16)(W(0x31c) + B(0x320)), 2, 4);
                    UH(0x1c6) = z;
                    t = gUnk_020005F8;
                    if (!t)
                        return;
                    Anim_SelectPaletteBank(anim, 0x1f4);
                    UH(0x1f2) = 0xe000;
                    *(u8 **)(s + 0x200) = t + 0xa0c;
                    ActorDebris_Show(W(0x33c));
                    PlayerEffect_SetPaletteSource(W(0x33c), 0xe001, 0);
                    W(0x308) = 4;
                } else {
                    s32 r;
                    if (RandomByte() < 4) {
                        if (B(0x323) == 0)
                            B(0x323) = 10;
                    }
                    if (B(0x323) == 0)
                        break;
                    r = Mod(W(0), 0x20);
                    Vec_Diff(&d, Ratatosk_GetPosition(), (struct CloneVec *)(s + 0x1d8));
                    if (r == 0) {
                        zero = r;
                        CpuSet((void *)&zero, &d, 0x05000002);
                    } else {
                        d.x = Div(d.x * r, 0x20);
                        d.y = 0;
                        d.z = Div(d.z * r, 0x20);
                    }
                    Vec_Sum(&e, &d, (struct CloneVec *)(s + 0x1d8));
                    e.y += 0x60;
                    SlotMotion_Initialize(motion);
                    Ratatosk_ConfigureParticle(motion, 10, 0, 0, 0, 0);
                    SceneParticles_SpawnAtPosition(motion, &e);
                    B(0x323)--;
                }
            }
            break;
        case 4: {
            s16 *p = &H(0x332);
            if (*p > 5) {
                u8 *c2 = s + 0xd4;
                *(s16 *)(c2 + 6) &= ~4;
            }
            if (*p > 30) {
                W(0x308) = 3;
                ctx[6] = RandomAngle();
                CTX_SPEED(ctx) = W(0x324);
                Bits_Set(1, s + 0x304);
                W(0x31c) = 5;
                Anim_LoadKeyframe(s + 0x1b8, s + 0x278, 5, 1);
            }
            (*p)++;
            break;
        }
        case 5: {
            u16 *p;
            if (W(0) & 1)
                SpriteParticle_CreateDefaultVariant4384(s + 0x14, 0x28, 0x20, 0xc, 4, 0x18,
                                                        RandomByte(), 0x20, 0x20, 3, 2, 0x200);
            {
                s16 y = H(0x1e) - (30 - (i * 30 - (W(0) - 0x2b2))) * 2;
                H(0x1da) = y;
            }
            p = &UH(0x340);
            if (Sprite_BlinkUntilExpired(s + 0x1c0, (s16)*p, 0x2d, 0x5a))
                return 1;
            (*p)++;
            break;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_082003BC);
#endif
