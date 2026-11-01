#include "contact_swarm.h"
#include "scene_vec.h"
#include "engine.h"
#include "player_cue_sequence.h"
struct P {
    u8 pad0[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    u32 unknown40C;
    const void *resource;
    const void *resource2;
    u8 pad3[0x458 - 0x418];
    u8 phase;
    u8 pad4;
    u16 timer;
    u8 pad5[0x4E8 - 0x45C];
    u16 scale;
    u8 pad6[0x58C - 0x4EA];
    void (*fn)(struct P *);
};
void Player_SetTetherMode(struct P *, s32, s32);
void sub_08238238(struct P *);
#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))
#define S(o) (*(s16 *)((u8 *)p + (o)))
// confidence=low: input-facing pose sequence exits when the action status becomes negative.
#define PlayerCue_UpdateFacingPose sub_08241714
void PlayerCue_UpdateFacingPose(struct P *p)
{
    if (B(0x457) != 0x17)
        Player_SetTetherMode(p, 0x17, 0);
    if (S(0x34C) >= 0) {
        u8 *direction = &p->dir;
        *direction = (H(0x34C) & 254) + 1;
        if (p->dir > 4) {
            s32 side = (8 - p->dir) >> 1;
            u8 *facing = &p->facing;
            *facing = side;
            p->flip = 1;
        } else {
            u32 side = p->dir >> 1;
            u8 *facing = &p->facing;
            *facing = side;
            p->flip = 0;
        }
    }
    Anim_SetSequence(&p->seq, (void *)p->resource, 2, p->facing, p->flip);
    PlayerCue_StepSequence(&p->sprite, &p->seq);
    if ((s8)B(0x4F3) < 0) {
        sub_08238238(p);
        Player_SetTetherMode(p, 0, 0);
    }
}
