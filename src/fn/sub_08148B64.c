#include "contact_swarm.h"
#include "player.h"
#include "render.h"
#include "player_cue_sequence.h"
// confidence=low: action pulses a spiral effect while alternating the folded facing.
struct ReleaseAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};

u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateFacingSide(struct Player *);
void Sound_PlaySong(u32);
void Player_UpdateSpiralDisorientation(struct Player *p)
{
    if (Player_ConsumeStateEntry(p) || !p->tetherFrame) {
        if (p->side)
            p->facing = 7 - p->heading * 2;
        else
            p->facing = p->heading * 2 + 1;
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
    if ((p->tetherFrame & 7) < 7 - (p->tetherFrame >> 2)) {
        if (p->facing > 3) {
            p->heading = (7 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    } else {
        struct Words {
            u32 a, b;
        };
        Player_AddControlFlags(p, 0x41);
        *(struct Words *)((u8 *)p + 0xAC) = *(struct Words *)((u8 *)p + 0x30);
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    p->spriteResource = 0x16F;
    p->actionVariantResource = (u32)(gPaletteBankData + (p->spriteResource << 5));
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x40C), 0,
                     p->heading, p->side);
    PlayerCue_StepSequence(&((struct ReleaseAnimation *)p)->sprite,
                           &((struct ReleaseAnimation *)p)->seq);
    {
        u32 offset = 0xAD6;
        u16 *parameter;
        parameter = (u16 *)((u8 *)p + offset);
        {
            register u32 value = 0x2DD;
            *parameter = value;
        }
    }
    {
        u32 durationOffset = (u32) & ((struct Player *)0)->actionEighteenDuration;
        struct Duration {
            u16 actionEighteenDuration;
        } *duration;
        duration = (struct Duration *)((u8 *)p + durationOffset);
        if ((duration->actionEighteenDuration += 3) > 32)
            duration->actionEighteenDuration = 32;
    }
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        Player_UpdateFacingSide(p);
        ActorSequence_Begin(p, 0, 0);
    }
}
