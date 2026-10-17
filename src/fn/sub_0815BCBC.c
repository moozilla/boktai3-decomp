#include "global.h"

struct Vec {
    s16 x, y, z, w;
};

struct P {
    u8 filler[0x30];
    struct Vec pos;
    u8 filler1[0x3A4 - 0x38];
    u8 dir;
    u8 filler2[0x418 - 0x3A5];
    u8 state;
    u8 filler3[0x58C - 0x419];
    void *fn;
    u8 filler4[0x5A4 - 0x590];
    u32 c;
};

extern const s16 gUnk_086149C4[];

void sub_0815AD78(struct P *);
void sub_0813AFEC(struct P *, s32, s32);
void Sound_PlaySong(s32);
void sub_082375FC(struct Vec *, s32, s32, s32, s32, s32, s32);
void sub_08151310(void);

static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

void sub_0815BCBC(struct P *p, s32 a, u32 c)
{
    struct Vec v;
    u32 angle;
    s32 id;

    v = p->pos;
    angle = ((p->dir + 5) & 7) << 5;
    v.x += trunc12(gUnk_086149C4[(angle + 0x40) & 0xFF] * 150);
    v.y += 0xE6;
    v.z += trunc12(gUnk_086149C4[angle] * 150);
    if (p->state == 6) {
        Sound_PlaySong(0x203);
        id = 0x51;
    } else {
        Sound_PlaySong(0xCD);
        id = 0x4B;
    }
    sub_082375FC(&v, id, 0, p->dir, 0x20, 0, a);
    p->c = c;
    sub_0815AD78(p);
    sub_0813AFEC(p, 3, 2);
    p->fn = sub_08151310;
}
