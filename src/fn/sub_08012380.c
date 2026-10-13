#include "animated_hazard.h"
// MATCHING_CONCESSION: register-pinning
s32 sub_08012058(void *, struct AnimatedHazard *);
void sub_0801204C(void *, struct AnimatedHazard *, u32);
static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
void AnimatedHazard_AnimateHold(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct HazardSprite *o;
    struct HazardAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    int done;
    if (sub_08012058(m, p)) {
        Anim_SetSequence(&p->anim, m->script, 1, 0, 0);
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
    if (p->timer >= p->hold)
        sub_0801204C(m, p, 5);
    Collider_Refresh(p->collider);
}
