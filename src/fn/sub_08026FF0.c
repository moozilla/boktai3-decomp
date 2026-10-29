#include "global.h"

extern const s16 gUnk_086149C4[];
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
    p->angle = angle;
    {
        s32 shifted = (p->angle + 0x60) >> 6;
        u32 mask = 3;
        mask &= shifted;
        p->facing = mask;
    }
    {
        s32 shifted = (p->angle + 0x70) >> 5;
        u32 mask = 7;
        mask &= shifted;
        p->facing8 = mask;
    }
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

/* Update the reaction sway and restore the directional state when it completes. */
void sub_08026FF0(struct Actor08026FF0 *base, int time)
{
    struct Actor08026FF0 *p = base;
    u8 *position = (u8 *)p + 0x48;
    s16 *velocity;
    s16 *minimum;
    s16 *maximum;
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
    if (time <= 15) {
        u32 angle;
        int speed;
        u8 *angleField = &p->angleSource;
        s32 initialSpeed = 16;
        velocity = (s16 *)(position + 20);
        angle = *angleField;
        speed = initialSpeed - time;
        {
            int x = f08026FF0_trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
            int z = 0;
            velocity[0] = x;
            velocity[1] = z;
        }
        velocity[2] = f08026FF0_trunc12(gUnk_086149C4[angle] * speed);
    }
    {
        s16 *value = &p->minScale;
        u32 old = (u16)*value;
        s32 current = *value;
        minimum = value;
        if (current > 23)
            *minimum = old - 24;
        else
            *minimum = 0;
    }
    {
        s16 *value = &p->maxScale;
        u32 old = (u16)*value;
        s32 current = *value;
        maximum = value;
        if (current > 23)
            *maximum = old - 24;
        else
            *maximum = 0;
    }
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
        void (*callback)(void *) = SceneActorAnimation_UpdateDirectionalCallback;
        u32 one = 1;
        u32 stateZero;
        u8 *active = &base->unkAB;
        stateZero = 0;
        *active = one;
        base->callback = callback;
        base->unk90 = stateZero;
        base->changed = one;
        base->unkA2 = stateZero;
        base->unkA4 = stateZero;
        if (p->unk341) {
            p->mode &= ~1;
            p->unk341 = 0;
            if (p->script) {
                Script_Call(p->script, 0);
                p->script = stateZero;
            }
        }
        {
            u8 variant = stateZero;
            *minimum = 0;
            *maximum = 0;
            p->variant = variant;
        }
    } else {
        u32 bit = 1;
        base->flags |= bit;
    }
}
