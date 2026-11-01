#include "global.h"
#include "display_sequence_inline.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Obj {
    u32 flags;
    u8 pad[12];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Piece {
    u8 active, pad1;
    u16 timer;
    s16 verticalScale, horizontalScale;
    u16 verticalStart, verticalDecay, horizontalStart, horizontalDecay;
    struct Vec direction, velocity;
    u8 resource[28];
    struct Obj sprite;
    struct Anim anim;
};
struct Manager {
    u8 pad[32];
    u32 timer;
    struct Piece pieces[32];
};
void Sprite_Remove(void *);
static inline int scale8(int a, int b)
{
    int x = a * b;
    if (x >= 0)
        return x >> 8;
    return -((-x) >> 8);
}
static inline int scale12(int a, int b)
{
    int x = a * b;
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline u32 frame_factor(u16 v)
{
    return DisplaySequence_MaskCommand(v, 15);
}
int AnimatedParticleManager_Update(struct Manager *m)
{
    struct Obj *sprites = &m->pieces[0].sprite;
    struct Piece *cursor = m->pieces, *base = cursor;
    int i;
    for (i = 31; i >= 0; sprites = (void *)((u8 *)sprites + 120), base++, cursor++, i--) {
        if (cursor->active) {
            struct Piece *p = base;
            struct Obj *o = sprites;
            struct Anim *a;
            const u16 *entry;
            register u16 n asm("r0");
            int done;
            if (cursor->timer >= cursor->horizontalStart) {
                p->horizontalScale = scale8(cursor->horizontalScale, cursor->horizontalDecay);
                p->velocity.x = scale12(p->direction.x, p->horizontalScale);
                p->velocity.z = scale12(p->direction.z, p->horizontalScale);
            }
            if (p->timer >= p->verticalStart) {
                p->verticalScale = scale8(p->verticalScale, p->verticalDecay);
                p->velocity.y = scale12(p->direction.y, p->verticalScale);
            }
            o->position.x += p->velocity.x;
            o->position.y += p->velocity.y;
            if (!(o->flags & 16))
                o->position.z += p->velocity.z;
            a = &p->anim;
            entry = a->data + a->frame;
            o->frame = *entry >> 6;
            {
                u32 input = a->flags;
                register u32 left asm("r2") = 1;
                u32 mask = 1;
                left &= input;
                if (left != (((*entry & 0x30) >> 4) & mask))
                    o->flags |= 4;
                else
                    o->flags &= ~4;
            }
            {
                register u32 input asm("r0") = a->flags;
                u32 mask = 2;
                register u32 left asm("r1") = (u8)(mask & input);
                if (left != (((*entry & 0x30) >> 4) & mask))
                    o->flags |= 8;
                else
                    o->flags &= ~8;
            }
            n = ++a->timer;
            if (n >= a->rate) {
                a->timer = 0;
                if (a->flags & 4) {
                    if (a->frame == 0) {
                        if (a->flags & 8) {
                            a->timer = a->rate;
                        } else {
                            a->frame = a->count - 1;
                        }
                        done = 1;
                    } else {
                        a->frame--;
                        done = 0;
                    }
                } else {
                    if (++a->frame >= a->count) {
                        if ((u8)(a->flags & 8)) {
                            a->timer = a->rate;
                            a->frame = a->count - 1;
                        } else {
                            a->frame = 0;
                        }
                        done = 1;
                    } else
                        done = 0;
                }
                entry = a->data + a->frame;
                a->factor = frame_factor(*entry);
                a->rate = (a->factor * a->scale) >> 6;
                if (!a->rate)
                    a->rate = 1;
            } else
                done = 0;
            if (done) {
                p->active = 0;
                Sprite_Remove(o);
            }
            p->timer++;
        }
    }
    m->timer++;
    return 0;
}
