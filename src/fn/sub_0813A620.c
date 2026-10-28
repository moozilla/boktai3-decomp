// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "background_controller.h"

/* tentative: positions two sprites relative to camera and steps their frame animation */

struct PairVec {
    s16 x, y, z, pad;
};

struct PairSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct PairVec position;
    u8 tail[8];
};

struct PairAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

struct PairPos {
    s32 x, y, z;
};

struct PairEntry {
    u8 pad0[0x1c];
    struct PairSprite sprite;
    struct PairAnim anim;
    struct PairPos *pos;
    s32 *offset;
    void *active;
};

struct PairManager {
    u8 task[0x18];
    struct PairEntry entries[2];
};

struct PairCamera {
    u8 pad[0x44];
    s32 x;
};

s32 SpritePair_UpdateAnimation(struct PairManager *m)
{
    s32 i = 0;
    u32 one = 1;
    u32 two = 2;
    for (; i < 2; i++) {
        struct PairEntry *e = &m->entries[i];
        struct PairSprite *o;
        struct PairAnim *a;
        const u16 *entry;
        if (!e->active)
            continue;
        e->sprite.position.x =
            (e->pos->x >> 12) - (s16)(((struct PairCamera *)gSceneCamera)->x - 16);
        e->sprite.position.y = ((e->pos->z + e->pos->y + *e->offset) >> 12) + 0x4e;
        o = &e->sprite;
        a = &e->anim;
        entry = e->anim.data + a->frame;
        o->frame = *entry >> 6;
        if ((u8)(one & a->flags) != (((*entry & 0x30) >> 4) & one))
            e->sprite.flags |= 4;
        else
            e->sprite.flags &= ~4;
        if ((u8)(two & a->flags) != (((*entry & 0x30) >> 4) & two))
            o->flags |= 8;
        else
            o->flags &= ~8;
        if (++a->timer >= a->rate) {
            a->timer = 0;
            if (a->flags & 4) {
                if (a->frame == 0) {
                    if (a->flags & 8)
                        a->timer = a->rate;
                    else
                        a->frame = a->count - 1;
                } else {
                    a->frame--;
                }
            } else {
                if (++a->frame >= a->count) {
                    if ((u8)(a->flags & 8)) {
                        a->timer = a->rate;
                        a->frame = a->count - 1;
                    } else {
                        a->frame = 0;
                    }
                }
            }
            entry = a->data + a->frame;
            {
                register u32 f asm("r1") = *entry;
                a->factor = f & 15;
            }
            a->rate = (a->factor * a->scale) >> 6;
            if (!a->rate)
                a->rate = one;
        }
    }
    return 0;
}
