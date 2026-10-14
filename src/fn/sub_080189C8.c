#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 pad[20];
    struct Vec position;
    u8 tail[8];
};
struct Orb {
    u8 state, angle;
    u16 rise, radius, timer;
    struct Sprite sprite;
};
struct Manager {
    u8 pad[24];
    int time, duration;
    u16 speed, limit, count, frame;
    void *resource;
    struct Vec position;
    struct Orb orbs[8];
};
extern const s16 gUnk_086149C4[];
void sub_080187E4(void *, void *);
void OrbitingParticles_Advance(void *, void *);
int sub_080187BC(void *);
void Task_Kill(void *);
static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
u8 *sub_0821D64C(int, int);
void *AnimData_FindEntry(int);
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_08217DE0(void *, void *, int);
void sub_08217EC4(void *, int, int);
void sub_08217EEC(void *, void *, int);
void sub_08217ECC(void *, int);
static inline u8 *cell(int i)
{
    u8 *p = sub_0821D64C(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
static inline int randomAngle(void)
{
    u32 offset, value;
    u16 *table;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    table = ((u16 *)RANDOM_TABLE_ADDR);
    value = *(u16 *)(offset + (u32)table);
    return (value & 127) - 32;
}
int OrbitingParticles_Initialize(struct Manager *m, struct Vec *source, int radius, int duration,
                                 int count, int type)
{
    int i, index, height, slope;
    struct Vec *pos;
    u8 *c;
    struct Orb *p;
    switch (type) {
        case 1:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 0;
            break;
        case 3:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 4;
            break;
        case 4:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 6;
            break;
        case 5:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 8;
            break;
        case 6:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 10;
            break;
        case 7:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 12;
            break;
        case 8:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 14;
            break;
        case 0:
        case 2:
        default:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 2;
            break;
    }
    m->position = *source;
    pos = &m->position;
    {
        int x = pos->x >> 8, z = pos->z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
    }
    c = cell(index);
    slope = *c >> 4;
    height = (*c & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)pos->z;
            break;
        case 2:
            height -= (u8)pos->x;
            break;
    }
    if (m->position.y < height)
        m->position.y = height;
    m->time = 0;
    m->duration = duration;
    m->speed = 20;
    m->limit = 400;
    if (count > 7)
        count = 8;
    m->count = count;
    p = m->orbs;
    for (i = 0; i <= 7; p++) {
        struct Sprite *s = &p->sprite;
        int angle, r;
        p->state = 0;
        p->angle = randomAngle();
        p->rise = 0;
        p->radius = radius;
        p->timer = i * 4;
        sub_08217DE0(s, m->resource, 1);
        sub_08217EC4(s, -4, -4);
        sub_08217EEC(s, m->resource, m->frame);
        sub_08217ECC(s, 1);
        angle = p->angle;
        r = p->radius;
        p->sprite.position.x = scale(gUnk_086149C4[(angle + 64) & 255] * r);
        p->sprite.position.y = 0;
        p->sprite.position.z = scale(gUnk_086149C4[angle] * r);
        p->sprite.position.y += p->rise;
        p->sprite.position.x += m->position.x;
        p->sprite.position.y += m->position.y;
        p->sprite.position.z += m->position.z;
        {
            int one = 1;
            ((u8 *)s)[15] = one;
            i += one;
        }
    }
    return 0;
}
