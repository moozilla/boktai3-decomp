// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "scene_vec.h"

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
static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                } else {
                    state->index = index - 1;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                } else {
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}
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
    Step(&p->sprite, &p->seq);
    {
        register u32 dummy asm("r1");
        u32 off270;
        u16 *q270;
        off270 = 0x270;
        q270 = (u16 *)((u8 *)p + off270);
        asm("" ::"r"(dummy));
        if (*q270 != 0) {
            register u32 o479 asm("r6") = 0x479;
            register u8 *a479 asm("r0");
            asm("" : "+r"(o479));
            a479 = (u8 *)p + o479;
            if (*a479 != 0) {
                register u32 o47c asm("r1") = 0x47C;
                register u8 *a47c asm("r0");
                s16 *target;
                s32 dx, dz;
                asm("" : "+r"(o47c));
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
