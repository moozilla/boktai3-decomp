#include "contact_swarm.h"
#include "player_cue_sequence.h"

/* Partial view of the player fields used by the recovery pose. */
struct PlayerCue {
    u8 unk_00[0x18];
    s32 slot;
    u8 unk_1C[0x14C - 0x1C];
    struct ContactSwarmSprite sprite;
    u8 unk_178[0x3A2 - 0x178];
    u8 heading, side, facing;
    u8 unk_3A5[0x3FC - 0x3A5];
    struct DisplaySequenceState sequence;
    u32 unk_40C;
    void *resource;
    u8 unk_414[0x458 - 0x414];
    u8 phase;
    u8 unk_459[0x4EC - 0x459];
    u16 countdown;
};
// confidence=low: provisional action interpretation from retained C and assembly.
static inline u32 Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st, u32 firstBit,
                       u32 pin)
{
    return PlayerCue_StepSequenceEnded(sp, st);
}

u32 Player_ConsumeStateEntry(struct PlayerCue *);
void Player_SetSecondaryActionFlags(struct PlayerCue *, u32);
void sub_08238238(struct PlayerCue *);
void Player_SetTetherMode(struct PlayerCue *, s32, s32);
void Player_PlayLocalSong(s32, u32);
// confidence=low: decays the stored recovery amount before playing the finishing pose.
void PlayerCue_UpdatePoseRecovery(struct PlayerCue *p)
{
    u32 phase;
    Player_SetSecondaryActionFlags(p, 8);
    phase = p->phase;
    switch (phase) {
        case 0:
            p->facing |= 1;
            if (p->facing > 4) {
                p->heading = (8 - p->facing) >> 1;
                p->side = 1;
            } else {
                p->heading = p->facing >> 1;
                p->side = 0;
            }
            Anim_SetSequence(&p->sequence, p->resource, 2, p->heading, p->side);
            Step(&p->sprite, &p->sequence, 1, 1);
            if (p->countdown <= 30) {
                p->countdown = 0;
                Player_PlayLocalSong(p->slot, 0x1B0);
                p->phase = 1;
            } else
                p->countdown -= 30;
            break;
        case 1:
            Anim_SetSequence(&p->sequence, p->resource, 3, p->heading, p->side);
            if (Step(&p->sprite, &p->sequence, phase, 2)) {
                sub_08238238(p);
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
}
