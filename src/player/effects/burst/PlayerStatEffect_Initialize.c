#include "player/effects/orbit_effect.h"
#define PlayerStatEffect_SetPhase PlayerBurst_SetStateCallback
#include "global.h"
#include "player/player.h"
#include "engine/engine.h"
void PlayerTransition_LoadAnimation(struct PlayerOrbitEffect *);
void PlayerTransition_InitCollider(struct PlayerOrbitEffect *);
void PlayerBurst_InitializeParticles(struct PlayerOrbitEffect *);
void PlayerOrbitEffect_UpdateIntro(struct PlayerOrbitEffect *);
void PlayerBurst_SetStateCallback(struct PlayerOrbitEffect *, void (*)(struct PlayerOrbitEffect *));
s32 Div(s32, s32);
s32 PlayerStatEffect_Initialize(struct PlayerOrbitEffect *p, struct Player *player)
{
    p->owner = player;
    PlayerTransition_LoadAnimation(p);
    PlayerTransition_InitCollider(p);
    PlayerBurst_InitializeParticles(p);
    PlayerBurst_SetStateCallback(p, PlayerOrbitEffect_UpdateIntro);
    gEngineFlags |= 1;
    p->targetStatA = p->owner->currentStatA >> 1;
    if (p->targetStatA == 0)
        p->targetStatA = 1;
    p->recoveryStep = Div(p->owner->currentStatA - p->targetStatA, 72) + 1;
    return 0;
}
