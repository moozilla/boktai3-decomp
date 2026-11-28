#include "scene/collision/contact_swarm.h"
#include "engine/math/scene_vec.h"
#include "engine/video/render.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/player.h"

void Player_SetTetherMode(struct Player *, s32, s32);
void LinkPlayer_FlushPendingEffects(struct Player *);
void Player_PlayLocalSong(s32, u32);
s32 Player_SetSecondaryActionFlags(struct Player *, u32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
// confidence=low: an alternate-form cue that counts up while sliding the pose window
void PlayerCue_UpdateSpiralPose(struct Player *p)
{
    if (p->tetherMode != 0xe) {
        Player_SetTetherMode(p, 0xe, 0);
        p->actionEighteenDuration = 0;
        LinkPlayer_FlushPendingEffects(p);
        Player_PlayLocalSong(p->slot, 0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 0x20, 0x38);
    }
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_SetSecondaryActionFlags(p, 0x41);
        p->animation.position = *(struct ContactPosition *)&p->position;
    }
    Player_SetAnimationIndex(p, 0x9a, 0x40);
    p->animation.palette = (u16 *)((u32)gPaletteBankData + 0x2DE0);
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
    p->actionEighteenParameter = 0x16f;
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        Player_SetTetherMode(p, 0, 0);
    }
}
