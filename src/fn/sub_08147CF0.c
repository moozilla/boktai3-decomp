#include "contact_swarm.h"
#include "player.h"
#include "player_cue_sequence.h"
// Tail address temporaries and register choices still differ.
// confidence=low: spiral particles accompany a timed release and palette cue.
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
void Player_UpdateSpiralRelease(struct Player *p)
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
        Sound_PlaySong(0x3A0);
    if ((p->tetherFrame & 7) < 7 - (p->tetherFrame >> 2)) {
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
    } else {
        Player_AddControlFlags(p, 0x41);
        p->colliderPosition = p->position;
        p->colliderPosition.y += *(u8 *)((u8 *)p + 0x466);
        if (p->facing > 3) {
            p->heading = (7 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
    p->actionEighteenParameter = 0x16F;
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, ((struct ReleaseAnimation *)p)->resource,
                     0, p->heading, p->side);
    PlayerCue_StepSequence(&((struct ReleaseAnimation *)p)->sprite,
                           &((struct ReleaseAnimation *)p)->seq);
    {
        u32 o = 0x178;
        void **q;
        q = (void **)((u8 *)p + o);
        {
            void *v = *q;
            AnimData_SelectPaletteBank(v, 0x2DD);
        }
    }
    {
        u32 o = 0x45A;
        u16 *tf;
        tf = (u16 *)((u8 *)p + o);
        if (++*tf > 31) {
            {
                u32 o2 = 0x464;
                u8 *q;
                q = (u8 *)p + o2;
                *q = 1;
            }
            {
                u32 o3 = 0x465;
                u8 *q;
                q = (u8 *)p + o3;
                *q = 3;
            }
            ActorSequence_Begin(p, 0, 0);
        }
    }
}
