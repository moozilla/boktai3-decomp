#include "global.h"
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
    s8 paletteActive, paletteMask;
    u16 actorId;
    u8 palette[32];
    void *actor;
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
void *sub_0821A520(int, int);
void sub_0800F9C4(void *, void *, int);
void sub_0800FA40(void *, void *, int);
void sub_08020CD4(void *, int, int);
void sub_08020D08(void *);
void *sub_08225984(int);
void sub_0801037C(void *);
void sub_0800FAB8(void *);
extern void (*gUnk_08604B48[])(void *, void *, int);
#define GroupObjects_Update sub_08010FD4
int GroupObjects_Update(struct Manager *m)
{
    int i;
    struct Piece *p;
    sub_0801037C(m);
    if (m->active) {
        if (m->actorId)
            m->actor = sub_08225984(m->actorId);
        if (m->paletteActive == 0) {
            if (++m->paletteMask >= 63) {
                m->paletteMask = 63;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 1) {
            if (--m->paletteMask <= 0) {
                m->paletteMask = 0;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 2) {
            if (++m->paletteMask > 31) {
                m->paletteMask = 32;
                m->paletteActive = -1;
            }
        }
        if (m->paletteActive >= 0) {
            void *first = gUnk_030042E4 + 0x5A60;
            void *second = gUnk_030042E4 + 0x5AA0;
            sub_08221110(m->palette, first, second, m->paletteMask, 6);
        }
        p = m->pieces;
        for (i = 0; i < 16; i++, p++) {
            gUnk_08604B48[p->state](m, p, i);
            sub_0800FAB8(p);
        }
        m->timer++;
    }
    return 0;
}
