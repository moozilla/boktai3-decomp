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
    *(u32 *)((u8 *)gPlayers[0] + 0x94) |= 1;
    ActorShadow_Enable((u8 *)gPlayers[0] + 0x700);
    if (Anim_Step(p->animation, p->animationData)) {
        *((u8 *)gPlayers[0] + 0x457) = 0;
        *((u8 *)gPlayers[0] + 0x3a4) = 5;
        *((u8 *)gPlayers[0] + 0x3a2) = 3;
        *((u8 *)gPlayers[0] + 0x3a3) = 1;
        PlayerBurst_SetStateCallback(p, PlayerBurst_UpdateShutdown);
    }
}
