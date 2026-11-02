#include "contact_swarm.h"
#include "scene_vec.h"
#include "render.h"
#include "player_cue_sequence.h"

struct PlayerSpiralPose {
    u8 unknown0[0x18];
    u32 song;
    u8 unknown1C[0x30 - 0x1C];
    struct SceneVec position;
    u8 unknown38[0xAC - 0x38];
    struct SceneVec savedPosition;
    u8 unknownB4[0xD4 - 0xB4];
    u32 animationResource;
    u8 unknownD8[0x14C - 0xD8];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    u32 unknown40C;
    const void *resource;
    const void *resource2;
    u8 pad3[0x457 - 0x418];
    u8 tetherMode;
    u8 phase;
    u8 pad4;
    u16 timer;
    u8 unknown45C[8];
    u8 completion0, completion1;
    u8 pad5[0x4E8 - 0x466];
    u16 scale;
    u8 pad6[0x58C - 0x4EA];
    void (*fn)(struct PlayerSpiralPose *);
    u8 unknown590[0xAD6 - 0x590];
    u16 trailResource, trailWindow;
};
void Player_SetTetherMode(struct PlayerSpiralPose *, s32, s32);
void LinkPlayer_FlushPendingEffects(struct PlayerSpiralPose *);
void Player_PlayLocalSong(s32, u32);
s32 Player_SetSecondaryActionFlags(struct PlayerSpiralPose *, u32);
s32 Player_SetAnimationIndex(struct PlayerSpiralPose *, s32, s32);
void SpiralTrail_CreateWithParticles(struct PlayerSpiralPose *, s32, s32, s32, s32);
// confidence=low: an alternate-form cue that counts up while sliding the pose window
void PlayerCue_UpdateSpiralPose(struct PlayerSpiralPose *p)
{
    if (p->tetherMode != 0xe) {
        Player_SetTetherMode(p, 0xe, 0);
        p->trailWindow = 0;
        LinkPlayer_FlushPendingEffects(p);
        Player_PlayLocalSong(p->song, 0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 0x20, 0x38);
    }
    if ((p->timer & 7) >= 7 - (p->timer >> 2)) {
        Player_SetSecondaryActionFlags(p, 0x41);
        p->savedPosition = p->position;
    }
    Player_SetAnimationIndex(p, 0x9a, 0x40);
    p->animationResource = ((u32)gPaletteBankData) + 0x2DE0;
    Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
    PlayerCue_StepSequence(&p->sprite, &p->seq);
    p->trailResource = 0x16f;
    if ((p->trailWindow += 3) > 32)
        p->trailWindow = 32;
    if (++p->timer > 31) {
        p->completion0 = 1;
        p->completion1 = 1;
        Player_SetTetherMode(p, 0, 0);
    }
}
