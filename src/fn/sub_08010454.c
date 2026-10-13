#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Piece {
    u8 active, state, done, angle;
    u16 timer;
    s8 direction;
    u8 pad7, speed, pad9;
    u16 targetY, duration, pade;
    struct Vec position, target;
    u8 pad20[32];
    u32 spriteFlags;
    u8 spritePad[24];
    struct Vec spritePosition;
    u8 pad64[20];
    void *palette;
    u8 pad7c[12];
    u8 transform[64];
};
struct Manager {
    u8 task[24];
    u32 active, timer;
    void *animation;
    struct Piece pieces[16];
    u8 paletteActive, paletteMask, padCA6[2];
    u8 palette[36];
    struct Vec origin;
};
extern u8 *gUnk_030042E4;
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern s16 gUnk_086149C4[];
void sub_08221110(void *, void *, void *, int, int);
int Mod(int, int);
void sub_0800FA5C(void *, void *, int);
void sub_08215284(void *, int);
void sub_08013424(void *);
static inline int randomValue(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct Vec *v, int angle, int radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void addPosition(struct Vec *d, struct Vec *s)
{
    struct Vec *dst = d;
    struct Vec *src = s;
    {
        u32 value = (u16)s->x;
        {
            register u32 other asm("r3") = (u16)d->x;
            value = value + other;
        }
        d->x = value;
    }
    dst->y += src->y;
    dst->z += src->z;
}
#define BoardPiece_Launch sub_08010454
void BoardPiece_Launch(struct Manager *m, struct Piece *p, int delay)
{
    struct Vec delta;
    if (p->active)
        p->active = 0;
    polar(&delta, p->direction, p->duration * 2);
    addPosition(&p->position, &delta);
    sub_0800FA5C(m, p, 0);
    if (p->timer >= delay + 2) {
        p->state = 1;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}
