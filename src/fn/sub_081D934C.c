#include "global.h"
struct Pair {
    u32 a;
    u32 b;
};
#define POS(n) (pp[n])
struct Inner {
    u8 pad0[0x14];
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
    u8 pad24[0x28];
    s32 f4c;
};
struct Boss {
    u8 pad0[0x18];
    struct Inner in;
    u8 pad7c[0x8c - 0x18 - sizeof(struct Inner)];
    u8 pad8c[0x1c];
    u16 cx;
    u8 padaa[2];
    u16 cz;
    u8 pada2[0x17e - 0xae];
    u16 height;
};
u8 *Explosion_CreateWithCollisionFlags(void *pos, u32 flag, s32 duration, s32 arg3, s32 arg4,
                                       s32 arg5, s32 arg6, s32 arg7, u32 arg8);
u8 *Explosion_Create(void *pos, u32 flag, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void SceneStimulus_AddTagged(s16, s16, s16, s16, u8, u16);

#define Boss_FinalExplosions sub_081D934C
s32 Boss_FinalExplosions(struct Boss *self)
{
    u8 *c = (u8 *)self + 0x8c;
    struct Inner *in = &self->in;
    struct Pair pos;
    u32 zero;
    s32 h;
    s16 *pp = (s16 *)&pos;
    u8 *src = (u8 *)self + 0xa8;
    pos = *(struct Pair *)src;
    h = self->height - 0x6c;
    zero = 0;
    POS(1) = h;
    Explosion_CreateWithCollisionFlags(&pos, 1, in->f14, in->f1c, in->f20, in->f18, 0x3c, in->f4c,
                                       zero);
    POS(0) = *(u16 *)(c + 0x1c) - 0x80;
    Explosion_Create(&pos, 1, 0, 0, in->f18, 0x3c, in->f4c, 5);
    POS(0) = *(u16 *)(c + 0x1c) + 0x80;
    Explosion_Create(&pos, 1, 0, 0, in->f18, 0x3c, in->f4c, 5);
    POS(0) = *(u16 *)(c + 0x1c);
    POS(2) = *(u16 *)(c + 0x20) - 0x80;
    Explosion_Create(&pos, 1, 0, 0, in->f18, 0x3c, in->f4c, 10);
    POS(2) = *(u16 *)(c + 0x20) + 0x80;
    Explosion_Create(&pos, 1, 0, 0, in->f18, 0x3c, in->f4c, 10);
    SceneStimulus_AddTagged(*(s16 *)src, *(s16 *)((u8 *)self + 0xac), 0x400, 0, 1, 0xB546);
    return 2;
}
