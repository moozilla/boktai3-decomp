#include "player/effects/orbit_effect.h"

void PlayerBurst_UpdateShutdown(struct PlayerOrbitEffect *);

void PlayerTransition_StepReveal(struct PlayerOrbitEffect *p)
{
    u16 *ticks = &p->phaseFrame;
    if (*ticks == 0) {
        Sound_PlaySong(0x1b0);
        (*ticks)++;
    }
    gPlayers[0]->animation.flags |= 1;
    ActorShadow_Enable(gPlayers[0]->shadow);
    if (Anim_Step(&p->drawable, &p->animationData)) {
        gPlayers[0]->tetherMode = 0;
        gPlayers[0]->facing = 5;
        gPlayers[0]->heading = 3;
        gPlayers[0]->side = 1;
        PlayerBurst_SetStateCallback(p, PlayerBurst_UpdateShutdown);
    }
}
