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
void Player_PlayLocalSong(s32, u32);
void Camera_RequestShake(u32);
void RisingSprite_Spawn(void *);
void sub_08238238(struct P *);
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
// confidence=low: a landing cue that slides toward a terrain target, then returns to idle
static inline s32 Scale3(s32 v)
{
    if (v >= 0)
        return v >> 3;
    return -(-v >> 3);
}
void PlayerCue_UpdateTerrainSlide(struct P *p)
{
    if (B(0x457) != 0x1d) {
        if (p->dir > 4) {
            p->facing = (8 - p->dir) >> 1;
            p->flip = 1;
        } else {
            p->facing = p->dir >> 1;
            p->flip = 0;
        }
        if (H(0x4C2) != 0) {
            Player_PlayLocalSong(W(0x18), H(0x4C2));
            H(0x4C2) = 0;
        }
        Player_SetTetherMode(p, 0x1d, 0);
    }
    Anim_SetSequence(&p->seq, p->resource, 2, p->facing, p->flip);
    PlayerCue_StepSequence(&p->sprite, &p->seq);
    {
        u32 dummy;
        u32 off270;
        u16 *q270;
        off270 = 0x270;
        q270 = (u16 *)((u8 *)p + off270);
        if (*q270 != 0) {
            u32 o479 = 0x479;
            u8 *a479;
            a479 = (u8 *)p + o479;
            if (*a479 != 0) {
                u32 o47c = 0x47C;
                u8 *a47c;
                s16 *target;
                s32 dx, dz;
                a47c = (u8 *)p + o47c;
                target = *(s16 **)a47c;
                dx = target[0] - *(s16 *)((u8 *)p + 0x30);
                dz = target[2] - *(s16 *)((u8 *)p + 0x34);
                H(0x38) = Scale3(dx);
                H(0x3C) = Scale3(dz);
                {
                    u16 *t = &H(0x45A);
                    if (*t <= 5) {
                        if ((*t & 1) == 0) {
                            Player_PlayLocalSong(W(0x18), 0x191);
                            RisingSprite_Spawn((u8 *)p + 0x30);
                        }
                        (*t)++;
                    }
                }
            } else {
                u32 ang = ((p->dir + 5) & 7) << 5;
                H(0x38) = -Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * H(0x270));
                H(0x3C) = -Scale(gUnk_086149C4[ang] * H(0x270));
            }
            {
                u16 *q = &H(0x270);
                Camera_RequestShake(*q);
                *q = (*q * 3) >> 2;
            }
        } else {
            H(0x270) = 0;
            B(0x479) = 0;
            B(0x4BA) = 0;
            if (B(0x5B9) == 3 && B(0x5BB) != 0) {
                Player_SetTetherMode(p, 0x16, 0);
            } else {
                sub_08238238(p);
                Player_SetTetherMode(p, 0, 0);
            }
        }
    }
}
