#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Body {
    u8 pad[0xc];
    struct Vec pos;
};
struct Actor {
    u8 pad0[0xa];
    u8 flashing;
    u8 padb[3];
    u16 flags;
    u8 pad10[0x4c];
    struct Body body;
    u8 pad70[0x64];
    u32 drawable;
    u8 padd8[0x1f0];
    s16 timer, duration;
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC, gUnk_030053F4;
u8 *sub_0821D64C(int, int);
void sub_08225B74(void *);
int DisplayAnimation_SetPosition(void *, void *);
int DisplayAnimation_UpdatePlayback(void *);
void sub_0802DE3C(struct Actor *, unsigned, int, int);
static inline u8 *cell(int i)
{
    u8 *p = sub_0821D64C(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
int sub_0802D9F8(void *unused, struct Actor *p)
{
    struct Body *body = &p->body;
    void *drawable = &p->drawable;
    sub_08225B74(body);
    {
        u32 mask = 0x400;
        if (!(p->flags & mask)) {
            struct Vec *pos = &body->pos;
            int x = pos->x >> 8, z = pos->z >> 8, index, height, slope;
            u8 *c;
            if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                index = 0;
            else
                index = gUnk_030052F4->rows[z] + x;
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
            body->pos.y = height;
        }
    }
    DisplayAnimation_SetPosition(drawable, &body->pos);
    DisplayAnimation_UpdatePlayback(drawable);
    if (p->timer > 0) {
        p->timer--;
        if (p->timer <= 3) {
            if (p->flashing && !(gUnk_030053F4 & 0x200))
                p->timer = p->duration;
            else {
                register s16 *timer asm("r0") = &p->timer;
                int zero;
                zero = 0;
                asm("" : "+r"(zero));
                *timer = 0;
                sub_0802DE3C(p, 8, 180, 180);
                p->flashing = zero;
            }
        }
    }
}
