#include "global.h"
#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/cues/player_cue.h"
#include "scene/collision/contact_swarm.h"
#include "engine/video/render.h"
#include "engine/video/display_sequence_inline.h"
#include "player/player_slot_status.h"
// confidence=low: action creates the spiral trail and advances its auxiliary sprite.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void Sound_PlaySong(u32);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
// confidence=low: provisional action interpretation from retained C and assembly.
void Player_UpdateSpiralTrailAction(struct Player *p)
{
    if (Player_ConsumeStateEntry(p) || !p->tetherFrame) {
        p->actionEighteenDuration = 0;
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
        Sound_PlaySong(0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    Player_SetStatusBits(&p->statusFlags, 0x100000);
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_AddControlFlags(p, 0x41);
        p->animation.position = *(struct ContactPosition *)&p->position;
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    p->animation.paletteBank = 0x16F;
    p->animation.palette = (u16 *)(gPaletteBankData + p->animation.paletteBank * 32);
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    p->actionEighteenParameter = 0x16F;
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        ActorSequence_Begin(p, 0, 0);
    }
}
