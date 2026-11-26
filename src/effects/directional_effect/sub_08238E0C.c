#include "global.h"

struct Pos {
    u32 a, b;
};

struct EffectState {
    u8 pad0[0x3a4];
    u8 dir;
    u8 pad3a5[0x7fc - 0x3a5];
    u8 sprite[0x1c];
    u16 x;
    u16 y;
    u8 pad_a[0x844 - 0x7fc - 0x20];
    u8 anim[0x14];
    u16 f858;
    u8 pad85a[2];
    u16 f85c;
};

extern const s16 gUnk_086149C4[];

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void Anim_SetSequence(void *, void *, s32, s32, s32);
void Player_ShowDirectionalEffect(void *);

// differs only in where 'movs r2, #0' is scheduled (the zero stores).
void DirectionalEffect_Spawn(struct EffectState *p, s32 unused, struct Pos *pos)
{
    u8 *sprite = p->sprite;
    s32 yy;
    s32 w;
    u16 z;
    s32 angle = ((p->dir + 5) & 7) << 5;
    *(struct Pos *)(sprite + 0x1c) = *pos;
    *(u16 *)(sprite + 0x1c) += scale(0xB4 * gUnk_086149C4[(angle + 64) & 255]);
    yy = scale(0xB4 * gUnk_086149C4[angle]);
    w = *(u16 *)(sprite + 0x20);
    w += yy;
    z = 0;
    *(u16 *)(sprite + 0x20) = w;
    p->f858 = z;
    p->f85c = z;
    Anim_SetSequence(p->anim, *(void **)((u8 *)p + 0x854), 2, 0, 0);
    Player_ShowDirectionalEffect(p);
}
