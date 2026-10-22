#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 96.1%
// Remaining: polar-output pointer scheduling, scratch registers and derived effect-offset constant.
#include "scene_vec.h"
struct PoisonHandBurst {
    u8 pad0[0x18];
    s32 frame;
    u8 pad1C[0x24];
    struct SceneVec position;
    u8 pad48[0xd8];
    s16 effectX, effectY, effectZ, unknown126;
    s16 countdown;
    u8 pad12A[2];
    s16 spread;
    u8 pad12E[0x16];
    s16 timer;
};
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void PoisonHand_RecordDefeat(void *);
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, s32, s32, u32, s32, u32, u32);
struct BurstVelocity {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 PoisonHand_NextSpread(const u16 *table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void PoisonHand_CopyPosition(struct SceneVec *dst, struct SceneVec *src)
{
    *dst = *src;
}
static inline void PoisonHand_RaiseOrigin(struct SceneVec *p)
{
    p->y += 0x80;
}

extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u16);
void PoisonHand_WaitForPlayerApproach(void);
struct SceneVec *Player_GetEffectivePosition(s32);
static inline s32 Scale(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 12;
    else
        r = -((-x) >> 12);
    return r;
}
// confidence=low: maintains the approach-triggered emergence animation and particle emission.
struct Velocity {
    s16 x, y, z, pad;
};
static inline void Polar(struct Velocity *v, s32 angle, s32 magnitude)
{
    register const s16 *sine asm("r1") = gUnk_086149C4;
    asm("" : "+r"(sine));
    v->x = Scale(sine[(angle + 64) & 255] * magnitude);
    v->y = 0;
    v->z = Scale(sine[angle] * magnitude);
}
static inline s32 Distance(struct SceneVec *a, struct SceneVec *b)
{
    s32 x = b->x - a->x, z = b->z - a->z;
    return x * x + z * z;
}
s32 PoisonHand_UpdateEmergence(struct PoisonHandBurst *input)
{
    struct PoisonHandBurst *p = input;
    struct SceneVec position;
    struct Velocity velocity;
    register s32 angle asm("r5");
    register s32 magnitude asm("r2");
    register u32 attributes asm("r8");
    register u32 palette asm("r6");
    register struct Velocity *v asm("sb");
    PoisonHand_CopyPosition(&position, &p->position);
    PoisonHand_RaiseOrigin(&position);
    angle = PoisonHand_NextSpread(gUnk_0203B400) & 255;
    magnitude = p->spread + (PoisonHand_NextSpread(gUnk_0203B400) & 15) - 8;
    asm("" : "+r"(angle), "+r"(magnitude));
    Polar(&velocity, angle, magnitude);
    v = &velocity;
    asm("" : "+r"(v));
    attributes = 0x8000;
    palette = 0x1c5;
    asm("" : "+r"(attributes), "+r"(palette));
    if (Mod(p->countdown, 10) == 0 && p->countdown > 90) {
        if (Mod(p->countdown, 20) == 0)
            SpriteEffectController_Create(&position, v, 0x61f9, 0x31f4, 8, 0x200, 9, palette, 2, 0,
                                          0, 0, p->effectX, p->effectY, attributes, p->effectZ, 0,
                                          0);
        else
            SpriteEffectController_Create(&position, v, 0x61f9, 0x31f4, 8, 0x400, 9, palette, 2, 0,
                                          0, 0, p->effectX, p->effectY, attributes, p->effectZ, 0,
                                          0);
    }
    if (p->countdown > 0) {
        p->countdown--;
        if (p->countdown == 30) {
            s32 *range = (s32 *)((u8 *)p + 0x11c);
            struct SceneVec *player = Player_GetEffectivePosition(0);
            if (*range < Distance(&p->position, player)) {
                Anim_RestartSequence((u8 *)p + 0x10c, *(void **)((u8 *)p + 0x168), 0x18, 0, 12);
                Anim_SetDurationScale((u8 *)p + 0x10c, 0x20);
            } else {
                p->countdown = 180;
                Sound_PlaySong(0x51c);
            }
        }
    } else {
        Anim_RestartSequence((u8 *)p + 0x10c, *(void **)((u8 *)p + 0x168), 0x17, 0, 0);
        *(void **)((u8 *)p + 0x108) = PoisonHand_WaitForPlayerApproach;
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", PoisonHand_UpdateEmergence);
#endif
