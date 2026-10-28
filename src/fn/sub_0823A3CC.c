// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "area_effect_link.h"
#include "ram.h"
typedef struct Pos8 {
    u32 a, b;
} Pos8;
void sub_08232264(void *, void *, s32, s32, s32);
void Shadow_Initialize(void *, void *, s32);
void *SceneParticlePool_EnsureSingleton(void);
void LinkPlayer_InitAnimationState(u8 *);
void Player_InitPrimaryAuraEffect(u8 *);
void Player_InitOrbitingAuraEffect(u8 *);
void Player_InitSixParticleEffects(u8 *);
void Player_InitConvergingParticles(u8 *);
void DirectionalEffect_Init(u8 *);

static inline s32 LocalPlayer(void)
{
    if (gUnk_020004B4 == NULL)
        return -1;
    return gUnk_020004B4->localPlayer;
}

// confidence=low: initializes a link player actor: shadow, effects and state bytes.
#define LinkPlayer_InitializeActor sub_0823A3CC
void LinkPlayer_InitializeActor(u8 *p)
{
    Pos8 *pos;
    s32 local;
    u32 zero;
    u32 four;
    sub_08232264(p + 0x5b8, pos = (Pos8 *)(p + 0xac), 0, 0, 1);
    *(Pos8 *)(p + 0x6f0) = *pos;
    {
        u16 *dest = (u16 *)(p + 0x6F8);
        zero = 0;
        asm("" : "+r"(zero));
        *dest = 0;
    }
    *(u16 *)(p + 0x6fa) = 0;
    *(u16 *)(p + 0x6fc) = 0;
    Shadow_Initialize(p + 0x700, p + 0x6f0, 0);
    SceneParticlePool_EnsureSingleton();
    LinkPlayer_InitAnimationState(p);
    Player_InitPrimaryAuraEffect(p);
    Player_InitOrbitingAuraEffect(p);
    Player_InitSixParticleEffects(p);
    Player_InitConvergingParticles(p);
    DirectionalEffect_Init(p);
    p[0xaf4] = four = 4;
    p[0xaf5] = four;
    p[0xaf6] = four;
    p[0xaf7] = four;
    p[0xaf8] = zero;
    p[0xaf9] = zero;
    p[0xafa] = zero;
    p[0xafb] = zero;
    *(u16 *)(p + 0xafc) = four;
    *(u16 *)(p + 0xafe) = four;
    *(u16 *)(p + 0xb00) = four;
    *(u16 *)(p + 0xb02) = four;
    local = LocalPlayer();
    if (*(s32 *)(p + 0x18) == local)
        p[0xaf0] = 0;
    else
        p[0xaf0] = 1;
    *(u16 *)(p + 0xae8) = 0;
    *(u16 *)(p + 0xaea) = 0;
    *(u16 *)(p + 0xaec) = 0;
    gPlayerBackgroundState = 0;
    gPlayerBackgroundMode = 0;
    gPlayerDirectionLatch = 1;
    gPlayerEffectPendingCount = 0;
    p[0x4f3] = 0xff;
}
