#include "scene/collision/contact_swarm.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/player.h"
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_UpdateActionFacingSide(struct Player *);
// confidence=low: input-facing pose sequence exits when the action status becomes negative.
void PlayerCue_UpdateFacingPose(struct Player *p)
{
    if (p->tetherMode != 0x17)
        Player_SetTetherMode(p, 0x17, 0);
    if ((s16)p->directionHistory[0] >= 0) {
        u8 *direction = &p->facing;
        *direction = (p->directionHistory[0] & 254) + 1;
        if (p->facing > 4) {
            s32 side = (8 - p->facing) >> 1;
            u8 *facing = &p->heading;
            *facing = side;
            p->side = 1;
        } else {
            u32 side = p->facing >> 1;
            u8 *facing = &p->heading;
            *facing = side;
            p->side = 0;
        }
    }
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 2, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
    if ((s8)p->movementAnimationState < 0) {
        Player_UpdateActionFacingSide(p);
        Player_SetTetherMode(p, 0, 0);
    }
}
