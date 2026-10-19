#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.8%
// foundry draft (worker best 87.4%); what differs is mostly register allocation and instruction order
struct Spr {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
};

struct Seq {
    u16 *commands;
    u8 flags;
    u8 frameCount;
    u8 weight;
    u8 duration;
    u16 index;
    u8 pad0a[2];
    u16 speed;
    u16 elapsed;
};

struct P {
    u8 pad0[0x20];
    u32 flags;
    u8 pad24[0x30 - 0x24];
    u8 effect[0x14c - 0x30];
    struct Spr spr;
    u8 pad15c[0x3fc - 0x15c];
    struct Seq seq;
};

extern u8 *gUnk_02000710;
u32 Anim_SetSequence(void *anim, void *data, u32 sequence, u32 row, u32 flags);
s32 RepeatedEffect_RequestNearby(void *);
void Sound_PlaySong(u32 song);
void Player_UpdateFacingSide(void *);
u32 Player_ConsumeStateEntry(u8 *p);
u8 Player_GetSmoothedDirection(void *);
s32 Player_GetWalkSpeed(void *p);
void Player_SetDirectionalVelocity(void *, s32);
void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);

static inline u8 global_bit(void)
{
    u32 mask = 1;
    if (*(u32 *)(gUnk_02000710 + 0x868) & mask)
        return TRUE;
    return FALSE;
}

void sub_08148FC0(u8 *p)
{
    struct Spr *spr;
    struct Seq *seq;
    volatile u16 *cmd;

    if (Player_ConsumeStateEntry(p))
        p[0x46a] = 0;
    *(u32 *)(p + 0x20) &= ~1;
    p[0x3a4] = Player_GetSmoothedDirection(p);
    Player_SetDirectionalVelocity(p, Player_GetWalkSpeed(p));
    Player_UpdateFacingSide(p);
    seq = (struct Seq *)(p + 0x3fc);
    Anim_SetSequence(seq, *(void **)(p + 0x410), 1, p[0x3a2], p[0x3a3]);
    spr = (struct Spr *)(p + 0x14c);
    cmd = (volatile u16 *)(seq->commands + seq->index);
    spr->frame = *cmd >> 6;
    {
        u32 f = seq->flags;
        u32 one = 1;
        u32 t = one;
        t &= f;
        if (t != (((*cmd & 0x30) >> 4) & one))
            spr->flags |= 4;
        else
            spr->flags &= ~4;
    }
    {
        u32 f = seq->flags;
        u32 two = 2;
        u32 t = two;
        t &= f;
        if ((u8)t != (((*cmd & 0x30) >> 4) & two))
            spr->flags |= 8;
        else
            spr->flags &= ~8;
    }
    {
        u32 mask = 0xFFFF;
        u32 dur;
        u32 e = seq->elapsed + 1;
        u32 sd;
        seq->elapsed = e & mask;
        dur = seq->duration;
        e = (u16)e;
        sd = dur;
        if (e >= dur) {
            seq->elapsed = 0;
            if (seq->flags & 4) {
                if (seq->index == 0) {
                    if (seq->flags & 8) {
                        seq->elapsed = sd;
                        goto after;
                    }
                    seq->index = seq->frameCount - 1;
                } else {
                    seq->index = seq->index - 1;
                }
            } else {
                u32 idx = seq->index + 1;
                seq->index = idx;
                if ((idx & mask) >= seq->frameCount) {
                    if ((u8)(seq->flags & 8)) {
                        seq->elapsed = sd;
                        seq->index = seq->frameCount - 1;
                    } else
                        seq->index = 0;
                }
            }
        after:
            cmd = (volatile u16 *)(seq->commands + seq->index);
            seq->weight = *cmd & 15;
            {
                s32 w = seq->weight;
                seq->duration = (seq->speed * w) >> 6;
            }
            if (!seq->duration)
                seq->duration = 1;
        }
    }
    if (*(u16 *)(p + 0x404) == 2) {
        u16 z = *(u16 *)(p + 0x40a);
        u8 *fx;
        if (z == 0) {
            fx = p + 0x30;
            if (RepeatedEffect_RequestNearby(fx)) {
                if (p[0x46a]) {
                    p[0x46a] = z;
                    Player_PlayPositionEffect(p, fx, 1, 1, fx, z, 0x163);
                } else {
                    p[0x46a] = 1;
                    Player_PlayPositionEffect(p, fx, 1, 1, fx, 0, 0x162);
                }
            } else {
                if (p[0x46a]) {
                    p[0x46a] = 0;
                    Sound_PlaySong(0xc7);
                } else {
                    p[0x46a] = 1;
                    Sound_PlaySong(0xc8);
                }
                if (global_bit())
                    Player_PlayPositionEffect(p, fx, 1, 2, 0, 0, 0);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08148FC0);
#endif
