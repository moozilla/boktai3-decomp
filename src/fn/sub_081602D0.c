#define PlayerStatEffect_SetPhase PlayerBurst_SetStateCallback
#include "global.h"
#include "player.h"
#include "engine.h"
struct PlayerStatEffect {
    u8 task[0x18];
    struct Player *player;
    u8 unknown1C[0xD8];
    u16 step;
    u16 targetStat;
};
void PlayerTransition_LoadAnimation(struct PlayerStatEffect *);
void PlayerTransition_InitCollider(struct PlayerStatEffect *);
void sub_08160108(struct PlayerStatEffect *);
void PlayerOrbitEffect_UpdateIntro(struct PlayerStatEffect *);
void PlayerBurst_SetStateCallback(struct PlayerStatEffect *, void (*)(struct PlayerStatEffect *));
s32 Div(s32, s32);
s32 PlayerStatEffect_Initialize(struct PlayerStatEffect *p, struct Player *player)
{
    p->player = player;
    PlayerTransition_LoadAnimation(p);
    PlayerTransition_InitCollider(p);
    sub_08160108(p);
    PlayerBurst_SetStateCallback(p, PlayerOrbitEffect_UpdateIntro);
    gEngineFlags |= 1;
    p->targetStat = p->player->currentStatA >> 1;
    if (p->targetStat == 0)
        p->targetStat = 1;
    p->step = Div(p->player->currentStatA - p->targetStat, 72) + 1;
    return 0;
}
