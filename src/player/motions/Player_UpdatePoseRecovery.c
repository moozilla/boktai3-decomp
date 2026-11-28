#include "scene/collision/contact_swarm.h"
#include "player/player.h"
#include "player/cues/player_cue.h"
#include "player/cutscene/player_cue_sequence.h"
// confidence=low: provisional action interpretation from retained C and assembly.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_AddControlFlags(struct Player *, u32);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
// confidence=low: decays the stored recovery amount before playing the finishing pose.
void Player_UpdatePoseRecovery(struct Player *p)
{
    Player_AddControlFlags(p, 8);
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
            PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
            if (p->actionCountdown <= 30) {
                p->actionCountdown = 0;
                Sound_PlaySong(0x1B0);
                p->tetherPhase = 1;
            } else
                p->actionCountdown -= 30;
            break;
        case 1:
            Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 3, p->heading,
                             p->side);
            if (PlayerCue_StepSequenceEnded(&PlayerCue_Graphics(p)->sprite,
                                            &p->cueCollider.sequence)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
