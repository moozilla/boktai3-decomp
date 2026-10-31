#include "player_cue_sequence.h"
#include "render.h"
#include "scene_vec.h"

// confidence=low: tentative player cue role; see callback body.
struct PlayerCueSpiral {
    u8 unknown0[0x30];
    struct SceneVec position;
    s16 velocityX;
    u16 unknown3A;
    s16 velocityZ;
    u8 unknown3E[0xAC - 0x3E];
    struct SceneVec previousPosition;
    u8 unknownB4[0xC6 - 0xB4];
    u16 spriteResource;
    u8 unknownC8[0xD4 - 0xC8];
    const void *paletteResource;
    u8 unknownD8[0x14C - 0xD8];
    struct ContactSwarmSprite sprite;
    u8 unknown178[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 heading, side, facing;
    u8 unknown3A5[0x3FC - 0x3A5];
    struct DisplaySequenceState sequence;
    const void *resource;
    u8 unknown410[0x458 - 0x410];
    u8 phase, unknown459;
    u16 timer;
    u8 unknown45C[8];
    u8 actionLatch464, actionLatch465;
    u8 unknown466[0x4E8 - 0x466];
    u16 scale;
    u8 unknown4EA[0x58C - 0x4EA];
    void (*updateCallback)(struct PlayerCueSpiral *);
    struct SceneVec destination;
    u16 speed;
    u8 unknown59A[3];
    s8 finalDir;
    u8 unknown59E[5];
    u8 pendingAnimationTrigger;
    u8 unknown5A4[0xAD6 - 0x5A4];
    u16 trailResourceIndex, trailExtent;
};
void ActorSequence_Begin(struct PlayerCueSpiral *, s32, s32);
void Player_AddControlFlags(struct PlayerCueSpiral *, u32);
void PlayerCue_UpdateHidden(struct PlayerCueSpiral *);
void PlayerCue_CompleteAction(struct PlayerCueSpiral *);
void PlayerCue_EnterStandUpdate(struct PlayerCueSpiral *);
void Player_RestartAnimationFromSetup(struct PlayerCueSpiral *);
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32);
void Player_RestartAnimationFromResource(struct PlayerCueSpiral *);
void Player_UpdateFacingSide(struct PlayerCueSpiral *);
u32 Player_SetAnimation(struct PlayerCueSpiral *, u32, u32, u32);
void SpiralTrail_CreateWithParticles(struct PlayerCueSpiral *, s32, void *, s32, s32);

#ifdef NONMATCHING
// fuzzy: 89.1%
// scope shape 5 (differences once low registers are set aside)
void PlayerCue_UpdateSpiralMotion(struct PlayerCueSpiral *p)
{
    if (p->timer == 0) {
        if (p->side)
            p->facing = 7 - (p->heading * 2);
        else
            p->facing = (p->heading * 2) + 1;
        p->trailExtent = 0;
        Sound_PlaySong(0x37F);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    if ((p->timer & 7) < (7 - (p->timer >> 2))) {
        if (p->facing > 3) {
            p->heading = (7 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    } else {
        Player_AddControlFlags(p, 0x41);
        p->previousPosition = p->position;
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    {
        u16 *destination = &p->spriteResource;
        u32 resource = 0x16F;
        *destination = resource;
        p->paletteResource = gPaletteBankData + (resource << 5);
    }
    Anim_SetSequence(&p->sequence, (void *)p->resource, 0, p->heading, p->side);
    PlayerCue_StepSequence(&p->sprite, &p->sequence);
    {
        u16 *destination = &p->trailResourceIndex;
        u32 resource = 0x2DD;
        *destination = resource;
    }
    {
        u32 value = p->trailExtent + 3;
        u16 counterMask;
        u32 timer;
        p->trailExtent = value;
        counterMask = 0xFFFF;
        if ((u16)(value & counterMask) > 32)
            p->trailExtent = 32;
        timer = p->timer + 1;
        p->timer = timer;
        if ((timer & counterMask) > 31) {
            p->actionLatch464 = 1;
            p->actionLatch465 = 1;
            Player_UpdateFacingSide(p);
            PlayerCue_EnterStandUpdate(p);
            p->pendingAnimationTrigger = 1;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", PlayerCue_UpdateSpiralMotion);
#endif
