#include "global.h"
#include "link_session_state.h"
#include "ram.h"
#include "player.h"
void sub_08232264(void *, void *, s32, s32, s32);
void Shadow_Initialize(void *, void *, s32);
void *SceneParticlePool_EnsureSingleton(void);
void LinkPlayer_InitAnimationState(struct Player *);
void Player_InitPrimaryAuraEffect(struct Player *);
void Player_InitOrbitingAuraEffect(struct Player *);
void Player_InitSixParticleEffects(struct Player *);
void Player_InitConvergingParticles(struct Player *);
void DirectionalEffect_Init(struct Player *);

static inline s32 LocalPlayer(void)
{
    if (gLinkSession == NULL)
        return -1;
    return gLinkSession->localPlayer;
}

// confidence=low: initializes a link player actor: shadow, effects and state bytes.
#define LinkPlayer_InitializeActor sub_0823A3CC
void LinkPlayer_InitializeActor(struct Player *p)
{
    s32 local;
    sub_08232264(&p->effectGroup, &p->previousPosition, 0, 0, 1);
    *(struct SceneVec *)p->effectPosition = p->previousPosition;
    p->resetValue6F8 = 0;
    p->resetValue6FA = 0;
    p->resetValue6FC = 0;
    Shadow_Initialize(p->shadow, p->effectPosition, 0);
    SceneParticlePool_EnsureSingleton();
    LinkPlayer_InitAnimationState(p);
    Player_InitPrimaryAuraEffect(p);
    Player_InitOrbitingAuraEffect(p);
    Player_InitSixParticleEffects(p);
    Player_InitConvergingParticles(p);
    DirectionalEffect_Init(p);
    p->effectModes[0] = 4;
    p->effectModes[1] = 4;
    p->effectModes[2] = 4;
    p->effectModes[3] = 4;
    p->effectCounters[0] = 0;
    p->effectCounters[1] = 0;
    p->effectCounters[2] = 0;
    p->effectCounters[3] = 0;
    p->effectLimits[0] = 4;
    p->effectLimits[1] = 4;
    p->effectLimits[2] = 4;
    p->effectLimits[3] = 4;
    local = LocalPlayer();
    if (p->slot == local)
        p->unkAF0 = 0;
    else
        p->unkAF0 = 1;
    p->movementRequestTarget.x = 0;
    p->movementRequestTarget.y = 0;
    p->movementRequestTarget.z = 0;
    gPlayerBackgroundState = 0;
    gPlayerBackgroundMode = 0;
    gPlayerDirectionLatch = 1;
    gPlayerEffectPendingCount = 0;
    p->movementAnimationState = 255;
}
