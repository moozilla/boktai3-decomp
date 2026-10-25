#include "global.h"

// confidence=low: directional state update; refreshes facing and sway velocity, decays the scale fields, and either restores the directional callback or sets the flags bit 0.

// confidence=low: directional state update; refreshes facing and sway velocity, decays the scale fields, and either restores the directional callback or sets the flags bit 0.

// confidence=low: directional state update; refreshes facing and sway velocity, decays the scale fields, and either restores the directional callback or sets the flags bit 0.

// confidence=low: directional state update; refreshes facing and sway velocity, decays the scale fields, and either restores the directional callback or sets the flags bit 0.

// confidence=low: directional state update; refreshes facing and sway velocity, decays the scale fields, and either restores the directional callback or sets the flags bit 0.

// confidence=low: directional state update; refreshes facing and sway velocity, decays the scale fields, and either restores the directional callback or sets the flags bit 0.
extern s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void Sound_PlaySongChecked(u32);
void sub_08022790(void *, s32);
s32 Script_Call(u32, void *);
void SceneActorAnimation_UpdateDirectionalCallback(void *);

struct Actor08026FF0 {
    u8 pad00[0x51];
    u8 angle; /* 0x51: facing angle, set by the directional helper */
    u8 pad52[0x3E];
    u32 unk90; /* 0x90 */
    u8 pad94[0x8];
    u32 flags; /* 0x9C */
    u8 padA0[0x2];
    u8 unkA2; /* 0xA2 */
    u8 padA3;
    u32 unkA4;  /* 0xA4 */
    u8 facing;  /* 0xA8 */
    u8 facing8; /* 0xA9 */
    u8 unkAA;
    u8 unkAB; /* 0xAB */
    u8 padAC[0x5];
    u8 changed; /* 0xB1 */
    u8 padB2[0x4E];
    void (*callback)(void *); /* 0x100 */
    u8 pad104[0x25D - 0x104];
    u8 unk25D; /* 0x25D */
    u8 pad25E[0x33C - 0x25E];
    u32 unk33C;     /* 0x33C */
    u8 unk340;      /* 0x340 */
    u8 unk341;      /* 0x341: pending sound flag */
    u8 angleSource; /* 0x342 */
    u8 variant;     /* 0x343 */
    s16 scale;      /* 0x344 */
    s16 minScale;   /* 0x346 */
    s16 maxScale;   /* 0x348 */
    s16 scaleStep;  /* 0x34A */
    u8 pad34C[0x388 - 0x34C];
    u32 script; /* 0x388 */
    u8 pad38C[0x39B - 0x38C];
    u8 unk39B; /* 0x39B */
    u8 pad39C[0x3AC - 0x39C];
    u32 mode; /* 0x3AC: bit 0 picks the alternate sequence */
};

static inline void f08026FF0_setFacing(struct Actor08026FF0 *p, int angle)
{
    u32 value;
    int shifted;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

static inline int f08026FF0_choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}

static inline int f08026FF0_trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

static inline void f08026FF0_decay(s16 *p)
{
    u32 old = (u16)*p;
    *p = *p > 23 ? old - 24 : 0;
}

#ifdef NONMATCHING
// fuzzy: 68.0%
// scope shape 33 (differences once low registers are set aside)
// confidence=low: directional state update; refreshes facing and sway velocity, decays the scale fields, and either restores the directional callback or sets the flags bit 0.
void sub_08026FF0(struct Actor08026FF0 *base, int time)
{
    int tick = time;
    struct Actor08026FF0 *p = base;
    u8 *position = (u8 *)p + 0x48;
    s16 *velocity;
    u32 zero;

    if (p->changed) {
        int sequence;
        p->changed = 0;
        f08026FF0_setFacing(p, p->angleSource + 0x80);
        sequence = f08026FF0_choose(p->mode, 49, 11);
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 2, 1,
                                              4);
        if (p->unk341)
            Sound_PlaySongChecked(0x34E);
        Sound_PlaySongChecked(0x34F);
    }
    if (tick <= 15) {
        u32 angle;
        int speed;
        angle = p->angleSource;
        velocity = (s16 *)(position + 20);
        speed = 16 - tick;
        {
            int x = f08026FF0_trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
            int z = 0;
            velocity[0] = x;
            velocity[1] = z;
        }
        velocity[2] = f08026FF0_trunc12(gUnk_086149C4[angle] * speed);
    }
    f08026FF0_decay(&p->minScale);
    f08026FF0_decay(&p->maxScale);
    p->unk39B = 8;
    zero = 0;
    p->unk33C = zero;
    p->unk340 = zero;
    {
        u32 mask = 12;
        base->flags |= mask;
    }
    if (p->unk341 && (time & 3) == 0)
        sub_08022790(p, 0);
    if (p->unk25D & 2) {
        p->unkAB = 1;
        p->callback = SceneActorAnimation_UpdateDirectionalCallback;
        p->unk90 = zero;
        p->changed = 1;
        p->unkA2 = 0;
        p->unkA4 = zero;
        if (p->unk341) {
            p->mode &= ~1;
            p->unk341 = 0;
            if (p->script) {
                Script_Call(p->script, 0);
                p->script = zero;
            }
        }
        p->minScale = 0;
        p->maxScale = 0;
        p->variant = 0;
    } else {
        u32 bit = 1;
        base->flags |= bit;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08026FF0);
#endif
