#include "scene/collision/contact_swarm.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/player.h"

void Player_SetTetherMode(struct Player *, s32, s32);
void Player_RestartAnimation(struct Player *p)
{
    p->statusFlags &= ~1;
    if (p->tetherMode)
        Player_SetTetherMode(p, 0, 0);
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
}
