#include "global.h"
#include "game_state.h"

struct Vec081976E4 {
    s16 x, y, z, w;
};
struct P081976E4 {
    u8 f0[0x54];
    u8 effects[0x5404 - 0x54];
    struct Vec081976E4 pos;
    u8 f1[0x71be - 0x540c];
    u8 b71be;
    u8 f2[0x7244 - 0x71bf];
    s16 timer;
    u8 f3[0x7248 - 0x7246];
    u8 active;
    u8 mode;
    u8 state;
    u8 b724b;
    u8 flags[9];
    u8 dead[9];
    u8 b725e;
    u8 f4[0x7268 - 0x725f];
    u16 maxLife;
    u16 h726a;
    u16 maxHp;
    u8 f5[0x7276 - 0x726e];
    s16 life;
    u8 f6[0x728a - 0x7278];
    u16 h728a;
    u16 h728c;
    u8 f7[0x7290 - 0x728e];
    struct Vec081976E4 v7290;
    struct Vec081976E4 v7298;
};
void sub_0818EBF4(void *);
void sub_08193514(void *);
void sub_081940F8(void *);
void sub_081944C4(void *);
void sub_08195A4C(void *, u32);

static inline void Fill32(void *dst, u32 *tmp, u32 size)
{
    *tmp = 0;
    CpuSet(tmp, dst, 0x05000000 | size / 4);
}

s32 sub_081976E4(struct P081976E4 *t)
{
    s32 i;
    u8 *p;
    u32 tmp;
    t->active = 1;
    t->pos.x = -0xe00;
    t->pos.y = 0;
    t->pos.z = -0x80;
    t->timer = 0;
    t->b725e = 0xff;
    {
        u8 z = 0;
        for (i = 8, p = &t->dead[8]; i >= 0; p--, i--)
            *p = z;
    }
    sub_0818EBF4(t->effects);
    sub_08193514(t);
    sub_081940F8(t);
    sub_081944C4(t);
    sub_08195A4C(t, 0);
    t->life = t->maxLife;
    *(s32 *)((u8 *)gUnk_02000710 + 0x61c) = (s16)t->maxLife;
    *(s32 *)((u8 *)gUnk_02000710 + 0x618) = t->life;
    t->flags[0] = 0;
    t->flags[1] = 0;
    t->b71be = 0;
    t->flags[2] = 0;
    t->flags[3] = 0;
    t->flags[4] = 0;
    t->flags[5] = 0;
    t->flags[6] = 0;
    t->flags[7] = 0;
    t->flags[8] = 0;
    t->h728a = t->maxHp;
    t->h728c = t->maxHp;
    Fill32(&t->v7290, &tmp, 8);
    Fill32(&t->v7298, &tmp, 8);
    return 0;
}
