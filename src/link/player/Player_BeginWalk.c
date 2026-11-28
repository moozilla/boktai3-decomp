#include "scene/collision/contact_swarm.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/player.h"

s32 Player_GetLinkedSmoothedDirection(struct Player *);
s32 Player_GetAdjustedMovementInterval(struct Player *);
void LinkPlayer_SetScaledDirectionalVelocity(struct Player *, s32);
void Player_UpdateActionFacingSide(struct Player *);
void Player_PlayLocalSong(u32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_BeginWalk(struct Player *p)
{
    if (p->tetherMode != 1) {
        p->footstepSide = 0;
        Player_SetTetherMode(p, 1, 0);
    }
    p->statusFlags &= ~1;
    p->facing = Player_GetLinkedSmoothedDirection(p);
    LinkPlayer_SetScaledDirectionalVelocity(p, Player_GetAdjustedMovementInterval(p));
    Player_UpdateActionFacingSide(p);
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 1, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
    if (p->cueCollider.sequence.index == 2 && p->cueCollider.sequence.elapsed == 0) {
        if (p->footstepSide) {
            p->footstepSide = 0;
            Player_PlayLocalSong(p->slot, 0xC7);
        } else {
            p->footstepSide = 1;
            Player_PlayLocalSong(p->slot, 0xC8);
        }
    }
}
