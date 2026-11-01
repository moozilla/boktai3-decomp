#include "contact_swarm.h"
#include "player.h"
#include "player_cue_sequence.h"
// confidence=low: provisional action interpretation from retained C and assembly.
static inline u32 Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st, u32 firstBit,
                       u32 pin)
{
    return PlayerCue_StepSequenceEnded(sp, st);
}

u32 Player_ConsumeStateEntry(struct Player *);
void Player_AddControlFlags(struct Player *, u32);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
// confidence=low: decays the stored recovery amount before playing the finishing pose.
void Player_UpdatePoseRecovery(struct Player *p)
{
    u32 phase;
    Player_AddControlFlags(p, 8);
    phase = *((u8 *)p + 0x458);
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
            Anim_SetSequence((void *)((u8 *)p + 0x3FC), *(void **)((u8 *)p + 0x410), 2, p->heading,
                             p->side);
            Step((void *)((u8 *)p + 0x14C), (void *)((u8 *)p + 0x3FC), 1, 1);
            if (*(u16 *)((u8 *)p + 0x4EC) <= 30) {
                *(u16 *)((u8 *)p + 0x4EC) = 0;
                Sound_PlaySong(0x1B0);
                *((u8 *)p + 0x458) = 1;
            } else
                *(u16 *)((u8 *)p + 0x4EC) -= 30;
            break;
        case 1:
            Anim_SetSequence((void *)((u8 *)p + 0x3FC), *(void **)((u8 *)p + 0x410), 3, p->heading,
                             p->side);
            if (Step((void *)((u8 *)p + 0x14C), (void *)((u8 *)p + 0x3FC), phase, 2)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
