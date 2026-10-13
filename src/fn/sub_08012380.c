#include "global.h"
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
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Piece {
    u8 active, state, entered, pad3;
    u16 duration, pad6;
    u32 timer;
    u8 padc[88];
    struct Obj sprite;
    u8 tail[8];
    struct Anim anim;
};
void sub_08220C8C(void *, void *, int, int, int);
int sub_08012058(void *, void *);
void sub_0801204C(void *, void *, int);
void sub_0822B3BC(int);
void sub_0821FE40(void *);
static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
#define AnimatedHazard_AnimateHold sub_08012380
void AnimatedHazard_AnimateHold(u8 *m, struct Piece *p)
{
    struct Obj *o;
    struct Anim *a;
    const u16 *entry;
    register u16 n asm("r0");
    int done;
    if (sub_08012058(m, p)) {
        sub_08220C8C(&p->anim, *(void **)(m + 0x34), 1, 0, 0);
    }
    o = &p->sprite;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
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
    if (p->timer >= *(u32 *)((u8 *)p + 12))
        sub_0801204C(m, p, 5);
    sub_0821FE40((u8 *)p + 0x10);
}
