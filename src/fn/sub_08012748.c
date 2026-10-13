#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 pad[12];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};
struct Hazard {
    u8 active, state, entered, pad3;
    u16 duration, pad6;
    u32 timer;
    u32 hold;
    u8 pad10[84];
    struct Sprite sprite;
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
u8 *sub_0821D64C(int, int);
void *sub_08012040(void);
void *sub_080126DC(void);
struct Hazard *sub_08012728(void *);
void sub_0821FF24(void *, void *, int);
void sub_0821FF50(void *, int, int);
void sub_0801204C(void *, void *, int);
static inline int cellIndex(struct Vec *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u8 *cell(int i)
{
    u8 *p = sub_0821D64C(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
int AnimatedHazard_Create(struct Vec *source, int damage, int hold)
{
    void *m = sub_08012040();
    struct Hazard *p;
    struct Vec *pos;
    void *effect;
    u8 *c;
    int height, slope;
    int zero;
    if (!m) {
        m = sub_080126DC();
        if (!m)
            return -1;
    }
    p = sub_08012728(m);
    if (!p)
        return -1;
    p->sprite.position = *source;
    {
        struct Vec *ground = &p->sprite.position;
        int x = ground->x >> 8, z = ground->z >> 8, index;
        pos = ground;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
        c = cell(index);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)ground->z;
                break;
            case 2:
                height -= (u8)ground->x;
                break;
        }
    }
    {
        s16 *h = &p->sprite.position.y;
        zero = 0;
        *h = height;
    }
    effect = (u8 *)p + 16;
    sub_0821FF24(effect, pos, 0);
    sub_0821FF50(effect, (u16)damage, 50);
    sub_0801204C(m, p, 2);
    p->timer = zero;
    p->hold = hold;
    p->active = 1;
    return 0;
}
