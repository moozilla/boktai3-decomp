#include "contact_swarm.h"
#include "player.h"
#include "player_cue_sequence.h"
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
// confidence=low: timed palette cue follows the spiral release.
void Player_UpdateSpiralAfterglow(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->actionEighteenDuration = 0;
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
        Sound_PlaySong(0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    {
        u32 mask = 0x100000;
        p->statusFlags = mask | p->statusFlags;
    }
    if (p->tetherFrame == 16)
        Sound_PlaySong(0x3A1);
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_AddControlFlags(p, 0x41);
        p->colliderPosition = p->position;
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
    p->actionEighteenParameter = 0x16F;
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x410), 0,
                     p->heading, p->side);
    PlayerCue_StepSequence(&((struct ReleaseAnimation *)p)->sprite,
                           &((struct ReleaseAnimation *)p)->seq);
    {
        u32 offset = 0x178;
        {
            void **resource = (void **)((u8 *)p + offset);
            {
                void *data = *resource;
                AnimData_SelectPaletteBank(data, 0x16F);
            }
        }
    }
    {
        u32 timerOffset = 0x45A;
        u16 *timer;
        timer = (u16 *)((u8 *)p + timerOffset);
        if (++*timer > 31) {
            u32 offset = 0x464;
            {
                u8 *flag = (u8 *)p + offset;
                *flag = 1;
            }
            {
                u32 latchOffset = 0x465;
                u8 *latch;
                latch = (u8 *)p + latchOffset;
                *latch = 4;
            }
            ActorSequence_Begin(p, 0, 0);
        }
    }
}
