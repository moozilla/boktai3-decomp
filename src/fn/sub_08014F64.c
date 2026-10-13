#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad[28];
    struct Vec position;
    u8 tail[8];
};
struct Group {
    u8 active, count, pad2[36];
    u8 lifetime[6], enabled[6];
    u16 speed[6];
    u8 pad3e[2];
    struct Vec velocity[6];
    struct Sprite sprites[6];
};
void sub_08214514(struct Sprite *);
static inline int scale(int a, int b)
{
    int v = a * b;
    if (v >= 0)
        return v >> 12;
    else
        return -((-v) >> 12);
}
#define FragmentGroup_Update sub_08014F64
void FragmentGroup_Update(void *m, struct Group *g)
{
    int i;
    g->count = 0;
    for (i = 0; i <= 5; i++) {
        struct Sprite *s = &g->sprites[i];
        if (g->enabled[i]) {
            s->position.x += scale(g->velocity[i].x, g->speed[i]);
            s->position.y += g->velocity[i].y;
            s->position.z += scale(g->velocity[i].z, g->speed[i]);
            g->velocity[i].y -= 2;
            if (--g->lifetime[i] == 255) {
                g->enabled[i] = 0;
                sub_08214514(s);
            } else
                g->count++;
        }
    }
    if (!g->count)
        g->active = 0;
}
