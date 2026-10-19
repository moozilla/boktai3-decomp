#define Hresvelgr_UpdateAlternatingOrbit sub_081A0888
#define gSceneAngleSineTable             gUnk_086149C4
#include "global.h"
/* Tentative Hresvelgr layout and behavior names, inferred from static consumers. */
typedef struct HresvelgrPosition {
    s16 x, y, z;
} HresvelgrPosition;
typedef struct HresvelgrActor {
    u8 pad0[0x54];
    HresvelgrPosition position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[15];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 padB2[0x5e];
    HresvelgrPosition *origin;
    u8 pad114[0x217];
    u8 side;
} HresvelgrActor;
extern s16 gUnk_086149C4[];
static inline int trunc12(int n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}
static inline void add(HresvelgrPosition *a, HresvelgrPosition *b)
{
    b->x += a->x;
    b->y += a->y;
    b->z += a->z;
}
void sub_081A0888(HresvelgrActor *p, int tick)
{
    u32 angle;
    int speed;
    s16 *v;
    HresvelgrPosition *pos;
    {
        u32 mask = 0x1c;
        p->flags |= mask;
    }
    if (p->changed) {
        p->changed = 0;
        p->side = (p->side + 1) & 1;
    }
    if (tick > 119) {
        angle = (p->side << 6);
        {
            u32 half = 128;
            v = (s16 *)&p->position;
            angle |= half;
        }
        speed = 1280;
        {
            int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
            int zero = 0;
            pos = &p->position;
            v[0] = x;
            v[1] = zero;
        }
        v[2] = trunc12(gUnk_086149C4[angle & 255] * speed);
        add(p->origin, pos);
        p->position.y = p->origin->y;
        p->finished = 1;
    }
}
