#include "contact_swarm.h"
#include "player_cue_sequence.h"

struct PRecover {
    u8 pad00[0x20];
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
};
void Player_SetTetherMode(struct PRecover *, s32, s32);
void Player_ConsumeStateEntry(struct PRecover *);

void sub_08148E4C(struct PRecover *p)
{
    p->flags20 &= ~1;
    Player_ConsumeStateEntry(p);
    Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
    PlayerCue_StepSequence(&p->sprite, &p->seq);
}
