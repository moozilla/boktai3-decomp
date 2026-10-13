#include "animated_hazard.h"
// MATCHING_CONCESSION: register-pinning

struct AnimatedHazardManager *sub_08012040(void);
void *sub_080126DC(void);
struct AnimatedHazard *sub_08012728(struct AnimatedHazardManager *);
void sub_0801204C(void *, struct AnimatedHazard *, u32);
static inline u8 *cell(int i)
{
    u8 *p = sub_0821D64C(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
int AnimatedHazard_CreateProximity(struct SceneVec *source, int damage, int delay, int hold)
{
    struct AnimatedHazardManager *m = sub_08012040();
    struct AnimatedHazard *p;
    struct SceneVec *pos;
    void *effect;
    u8 *c;
    int height, slope;
    int zero;
    if (!m) {
        m = sub_080126DC();
        if (!m)
            return -1;
    }
    p = sub_08012728(m);
    if (!p)
        return -1;
    p->sprite.position = *source;
    {
        struct SceneVec *ground = &p->sprite.position;
        int x = ground->x >> 8, z = ground->z >> 8, index;
        pos = ground;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
        c = cell(index);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)ground->z;
                break;
            case 2:
                height -= (u8)ground->x;
                break;
        }
    }
    {
        s16 *h = &p->sprite.position.y;
        zero = 0;
        *h = height;
    }
    effect = p->collider;
    Collider_SetPosition(effect, pos, 0);
    {
        register int d asm("r9") = damage;
        sub_0821FF58(effect, d, 50, 0, 8, 30);
    }
    sub_0801204C(m, p, 1);
    Anim_SetSequence(&p->anim, m->script, 0, 0, zero);
    {
        struct HazardSprite *o = &p->sprite;
        struct HazardAnim *a;
        const u16 *entry;
        register u16 n asm("r0");
        int done;
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
    }
    AnimData_SelectTiles(m->animData, 0x33F);
    p->timer = 0;
    p->hold = hold;
    asm("" : "+r"(delay));
    p->duration = delay;
    p->active = 1;
    return 0;
}
