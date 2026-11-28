#include "scene/collision/contact_swarm.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/player.h"

// confidence=low: provisional action interpretation from retained C and assembly.
void Player_SetSecondaryActionFlags(struct Player *, u32);
void Player_UpdateActionFacingSide(struct Player *);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_PlayLocalSong(s32, u32);
// confidence=low: decays the stored recovery amount before playing the finishing pose.
void PlayerCue_UpdatePoseRecovery(struct Player *p)
{
    Player_SetSecondaryActionFlags(p, 8);
    switch (p->tetherPhase) {
        case 0:
            p->facing |= 1;
            if (p->facing > 4) {
                p->heading = (8 - p->facing) >> 1;
                p->side = 1;
            } else {
                p->heading = p->facing >> 1;
                p->side = 0;
            }
            Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 2, p->heading,
                             p->side);
            PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                                   &p->cueCollider.sequence);
            if (p->actionCountdown <= 30) {
                p->actionCountdown = 0;
                Player_PlayLocalSong(p->slot, 0x1B0);
                p->tetherPhase = 1;
            } else
                p->actionCountdown -= 30;
            break;
        case 1:
            Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 3, p->heading,
                             p->side);
            if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                                            &p->cueCollider.sequence)) {
                Player_UpdateActionFacingSide(p);
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
}
