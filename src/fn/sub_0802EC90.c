// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 88.8%
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad0[7];
    u8 done;
    u8 pad8[1];
    u8 entered;
    u8 pada[4];
    u16 flags;
    u8 pad10[8];
    u8 phase;
    u8 facing4;
    u8 facing8;
    u8 targetAngle;
    u8 pad1c[56];
    struct Vec target;
    u8 body[9];
    u8 angle;
    u8 pad66[2];
    struct Vec pos;
    struct Vec velocity;
    u8 pad78[92];
    u32 drawable;
    u8 padd8[80];
    struct Vec *sourcePos;
    u8 pad12c[52];
    struct Vec *drawPos;
    u8 pad164[0x88];
    u8 modeA, modeB;
    u8 pad1ee[2];
    u8 cap;
    u8 pad1f1[0x17];
    int speed;
    u32 speedSquared;
    u8 pad210[212];
    int (*callback)(struct Actor *);
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
u8 *TerrainModifiers_FindCellOverride(int, int);
extern const s16 gUnk_086149C4[];
int Actor_PlayAnimationEntry(struct Actor *, int, int, int, int);
void Actor_PlayFootstepSound(struct Actor *, int);
void DisplayAnimation_SetFlags(void *, int);
void DisplayAnimation_ClearFlags(void *, int);
void DisplayAnimation_SetPalette(void *, int, int, int);
void Sound_PlaySong(int);
int Math_Atan2(int, int);
static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
static inline int height(struct Vec *pos)
{
    int x = pos->x >> 8, z = pos->z >> 8, index;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    return (*cell(index) & 15) << 8;
}
void sub_0802EC90(void *unused, struct Actor *p, int ticks)
{
    if (p->entered) {
        p->entered = 0;
        p->done = 0;
        {
            void *anim = &p->drawable;
            DisplayAnimation_SetFlags(anim, 2);
            DisplayAnimation_SetPalette(anim, 0x40, 0x40, 0);
        }
        Sound_PlaySong(0x201);
    }
    if ((u8)p->callback(p) != 1) {
        int a = 0x40;
        int b = 0;
        switch (p->phase) {
            case 0: {
                u32 k = 200;
                asm("" : "+r"(k));
                b = (8 - ticks) * k + 0xa0;
                {
                    struct Vec *dp = *(struct Vec **)((u8 *)p + (k + 0x98));
                    int h = height(dp);
                    p->drawPos->y = h + b;
                    asm("" ::"r"(h));
                }
                if (ticks > 7)
                    p->phase++;
                DisplayAnimation_SetPalette(&p->drawable, 1, 0x80, 0);
                break;
            }
            case 1: {
                int d = 15 - ticks;
                int c = a - d * 4;
                if (ticks > 8) {
                    a = (24 - ticks) << 3;
                    b = (d * 4 + d) * 4;
                }
                if (ticks > 14)
                    p->phase = 2;
                {
                    int h = height(p->drawPos);
                    p->drawPos->y = h + b;
                    asm("" ::"r"(h));
                }
                DisplayAnimation_SetPalette(&p->drawable, c, a, 0);
                break;
            }
            case 2:
                DisplayAnimation_ClearFlags(&p->drawable, 2);
                p->done = 1;
                break;
        }
        {
            u8 ang = p->targetAngle;
            int zero = 0;
            p->angle = ang;
            p->facing8 = ((ang + 112) >> 5) & 7;
            p->facing4 = ((ang + 96) >> 6) & 3;
            Actor_PlayAnimationEntry(p, 0, 2, zero, 0);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0802EC90);
#endif
