#include "global.h"
struct Spark {
    u32 flags;
    u8 pad4[0x18 - 4];
    s16 x, y, z;
    u8 pad1E[0x28 - 0x1E];
    s16 vx, vy, vz;
    u8 pad2E[2];
    u8 active;
    u8 timer;
    u8 baseFrame;
    u8 pad33;
};
struct SparkBurst {
    u8 pad0[0x868];
    void *frames;
    u8 active;
    u8 pad86D[3];
    struct Spark sparks[6];
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
// confidence=low: moves each live spark by its velocity, switches its frame at tick 6 and
// hides it after tick 11; the burst ends when no spark is left.
void sub_08238FE0(struct SparkBurst *p)
{
    s32 live, i;
    if (!p->active)
        return;
    live = 0;
    for (i = 0; i < 6; i++) {
        if (p->sparks[i].active) {
            struct Spark *s = &p->sparks[i];
            struct Spark *m = s;
            if (++s->timer > 11) {
                s->flags |= 1;
                s->active = 0;
            } else {
                if (s->timer == 6)
                    SpriteDrawable_SelectTileFrame(s, p->frames, s->baseFrame + 1);
                m->x += m->vx;
                m->y += m->vy;
                m->z += m->vz;
                live++;
            }
        }
    }
    if (live == 0)
        p->active = 0;
}
