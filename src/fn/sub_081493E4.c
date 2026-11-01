#include "contact_swarm.h"
#include "player.h"
#include "player_cue_sequence.h"
// confidence=low: provisional action interpretation from retained C and assembly.
// Remaining differences: instruction selection and register allocation.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
struct ReleaseAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
// confidence=low: holds the auxiliary pose until the movement latch clears.
void Player_UpdateFacingSide(struct Player *);
void Player_UpdateMovementPose(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    }
    p->statusFlags &= ~1;
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x410), 2,
                     p->heading, p->side);
    PlayerCue_StepSequence(&((struct ReleaseAnimation *)p)->sprite,
                           &((struct ReleaseAnimation *)p)->seq);
    {
        u32 offset = 0x5BB;
        {
            u8 *latch = (u8 *)p + offset;
            if (!*latch) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
        }
    }
}
