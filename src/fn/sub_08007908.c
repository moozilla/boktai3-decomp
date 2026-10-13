#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Obj {
    u32 flags;
    u8 pad[12];
    u16 frame;
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Position {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
struct Trigger {
    s16 id, kind;
    s8 actor;
    u8 type, flags, flip;
    u8 pad8[16];
    u32 timer;
    u8 pad1C[4];
    struct Obj *obj;
    u8 pad24[4];
    struct Anim *anim;
    struct Position *position;
    u8 pad30[4];
    u16 script;
    u8 pad36[14];
    void (*update)(void *, struct Trigger *);
};
static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
struct Manager {
    u8 pad[28];
    void *resource;
};
extern struct Manager *gUnk_02000030;
void Trigger_UpdateIdleAnimation(void *, struct Trigger *);
void sub_08220C8C(struct Anim *, void *, u32, u32, u32);
extern u32 gUnk_030053F4;
s32 sub_0822D490(s32, s32);
void Trigger_UpdateLockedPause(void *, struct Trigger *);
void Trigger_UpdateActivation(void *, struct Trigger *);
void sub_08017044(s32, s32, s32, s32, struct Position *, struct Position *, s32, struct Trigger *);
void Trigger_UpdateOpeningAnimation(struct Manager *manager, struct Trigger *p)
{
    struct Obj *o;
    struct Anim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    struct Position raised;
    o = p->obj;
    a = p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
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
    if (p->flip)
        p->obj->flags |= 4;
    if (done) {
        raised = *p->position;
        raised.y += 128;
        if (!(gUnk_030053F4 & 2048) && sub_0822D490(p->id, p->kind) == 0) {
            if (p->id <= 5)
                sub_08017044(p->id, p->kind, 1, p->actor, &raised, &raised, 24, p);
            p->update = Trigger_UpdateLockedPause;
        } else {
            if (p->id <= 5)
                sub_08017044(p->id, p->kind, 0, p->actor, &raised, &raised, 24, p);
            p->update = Trigger_UpdateActivation;
        }
        p->timer = 0;
        sub_08220C8C(p->anim, gUnk_02000030->resource, 6, 0, 0);
    } else
        p->timer++;
}
