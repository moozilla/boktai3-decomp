#include "contact_swarm.h"
#include "player_cue_sequence.h"

struct PRecover {
    u8 pad000[0x18];
    u32 song;
    u8 pad00[0x20 - 0x1C];
    u32 flags20;
    u8 pad0[0x14C - 0x24];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    u32 unknown40C;
    const void *resource;
    const void *resource2;
    u8 pad3[0x457 - 0x418];
    u8 tetherActive;
    u8 pad4[0x46A - 0x458];
    u8 stepSoundFlag;
};
s32 sub_0823C4B4(struct PRecover *);
s32 Player_GetAdjustedMovementInterval(struct PRecover *);
void Player_SetDirectionalVelocity_08238300(struct PRecover *, s32);
void sub_08238238(struct PRecover *);
void Player_PlayLocalSong(u32, s32);
void Player_SetTetherMode(struct PRecover *, s32, s32);
void Player_BeginWalk(struct PRecover *p)
{
    if (p->tetherActive != 1) {
        p->stepSoundFlag = 0;
        Player_SetTetherMode(p, 1, 0);
    }
    p->flags20 &= ~1;
    p->dir = sub_0823C4B4(p);
    Player_SetDirectionalVelocity_08238300(p, Player_GetAdjustedMovementInterval(p));
    sub_08238238(p);
    Anim_SetSequence(&p->seq, p->resource, 1, p->facing, p->flip);
    PlayerCue_StepSequence(&p->sprite, &p->seq);
    if (({
            u32 o = 0x404;
            u8 *q;
            q = (u8 *)p + o;
            *(u16 *)q;
        }) == 2 &&
        p->seq.elapsed == 0) {
        if (p->stepSoundFlag) {
            p->stepSoundFlag = 0;
            Player_PlayLocalSong(p->song, 0xC7);
        } else {
            p->stepSoundFlag = 1;
            Player_PlayLocalSong(p->song, 0xC8);
        }
    }
}
