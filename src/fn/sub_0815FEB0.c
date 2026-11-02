#include "global.h"
#include "player.h"
#define PlayerTransition_SetPhase     PlayerBurst_SetStateCallback
#define PlayerTransition_FinishReveal PlayerBurst_UpdateShutdown
struct PlayerTransition {
    u8 unknown0[0x1c];
    u8 animationData[0x20];
    u8 animation[0xb6];
    u16 phaseTicks;
};
void Sound_PlaySong(u32);
void ActorShadow_Enable(void *);
s32 Anim_Step(void *, void *);
void PlayerBurst_UpdateShutdown(void *);
void PlayerBurst_SetStateCallback(void *, void (*)(void *));
void PlayerTransition_StepReveal(struct PlayerTransition *p)
{
    u16 *ticks = &p->phaseTicks;
    if (*ticks == 0) {
        Sound_PlaySong(0x1b0);
        (*ticks)++;
    }
    gPlayers[0]->animation.flags |= 1;
    ActorShadow_Enable(gPlayers[0]->shadow);
    if (Anim_Step(p->animation, p->animationData)) {
        gPlayers[0]->tetherMode = 0;
        gPlayers[0]->facing = 5;
        gPlayers[0]->heading = 3;
        gPlayers[0]->side = 1;
        PlayerBurst_SetStateCallback(p, PlayerBurst_UpdateShutdown);
    }
}
