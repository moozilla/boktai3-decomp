#include "contact_swarm.h"
#include "scene_vec.h"
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
void Player_EmitPositionEffect(struct P *, const void *, s32, u32, const void *, u32, u32);
extern const s16 gUnk_086149C4[];
struct PackedVec {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16, pad : 16;
};
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))
#define W(o) (*(u32 *)((u8 *)p + (o)))
// confidence=low: a cue that emits a displaced positional effect, then advances its pose
void PlayerCue_UpdateOffsetBurst(struct P *p)
{
    if (B(0x457) != 4) {
        struct PackedVec pt;
        u32 ang;
        pt = *(struct PackedVec *)((u8 *)p + 0x30);
        ang = ((p->dir + 5) & 7) << 5;
        pt.x += Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * 64);
        pt.y += 0x40;
        pt.z += Scale(gUnk_086149C4[ang] * 64);
        Player_SetTetherMode(p, 4, 0);
        Player_EmitPositionEffect(p, (u8 *)p + 0x30, 1, 1, &pt, 0, 0x3A1);
    }
    Anim_SetSequence(&p->seq, p->resource, 0, p->facing, p->flip);
    PlayerCue_StepSequence(&p->sprite, &p->seq);
    p->timer++;
    if (p->timer > 0xf)
        Player_SetTetherMode(p, 0, 0);
}
