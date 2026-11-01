// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_sequence_inline.h"
struct Obj {
    u32 flags;
    u8 pad[12];
    u16 frame;
    u8 pad12[10];
    s16 x, y, z, pad2;
    u8 tail[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Entry {
    u32 timer;
    u8 pad4[0x14];
    s32 x, y, z;
    u8 pad24[4];
    struct Anim anim;
    u8 pad38[2];
    u16 lifetime;
    u8 pad3c[8];
    u32 callback;
    s32 vx, vy, vz;
    u8 pad54[0x10];
    u32 script;
    u8 pad68[4];
    u8 pad6c[0x1c];
    struct Obj sprite;
    u8 padtail[0xb8 - 0x88 - sizeof(struct Obj)];
};
struct Pool {
    u8 pad[0x18];
    u32 mask;
    struct Entry e[32];
};
void sub_081FEDE8(struct Pool *, struct Entry *, s32);
s32 sub_08249240(struct Entry *, s32, u32);
static inline u32 frame_factor(u16 v)
{
    return DisplaySequence_MaskCommand(v, 15);
}
s32 SpriteParticle_UpdateManager(struct Pool *pool)
{
    struct Entry *base = pool->e, *cursor;
    s32 i;
    for (i = 0, cursor = base; i <= 31; i++, cursor++, base++) {
        if (pool->mask & (1 << i)) {
            struct Obj *o = &base->sprite;
            int done = 0;
            struct Anim *a;
            const u16 *entry;
            register u16 n asm("r0");
            if (sub_08249240(base, i, cursor->callback) == 1) {
                sub_081FEDE8(pool, base, i);
                continue;
            }
            cursor->x += cursor->vx;
            cursor->y += cursor->vy;
            cursor->z += cursor->vz;
            o->x = cursor->x >> 12;
            o->y = cursor->y >> 12;
            o->z = cursor->z >> 12;
            if (cursor->script) {
                int d;
                struct Obj *s = o;
                a = &base->anim;
                entry = cursor->anim.data + cursor->anim.frame;
                s->frame = *entry >> 6;
                {
                    u32 input = a->flags;
                    register u32 left asm("r2") = 1;
                    u32 mask = 1;
                    left &= input;
                    if (left != (((*entry & 0x30) >> 4) & mask))
                        s->flags |= 4;
                    else
                        s->flags &= ~4;
                }
                {
                    register u32 input asm("r0") = a->flags;
                    u32 mask = 2;
                    u32 left = (u8)(mask & input);
                    if (left != (((*entry & 0x30) >> 4) & mask))
                        s->flags |= 8;
                    else
                        s->flags &= ~8;
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
                            d = 1;
                        } else {
                            a->frame--;
                            d = 0;
                        }
                    } else {
                        if (++a->frame >= a->count) {
                            if ((u8)(a->flags & 8)) {
                                a->timer = a->rate;
                                a->frame = a->count - 1;
                            } else {
                                a->frame = 0;
                            }
                            d = 1;
                        } else
                            d = 0;
                    }
                    entry = a->data + a->frame;
                    a->factor = frame_factor(*entry);
                    a->rate = (a->factor * a->scale) >> 6;
                    if (!a->rate)
                        a->rate = 1;
                } else
                    d = 0;
                done = d;
            }
            if (cursor->timer > cursor->lifetime || done == 1)
                sub_081FEDE8(pool, base, i);
            cursor->timer++;
        }
    }
    return 0;
}
