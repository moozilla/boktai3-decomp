#include "breakable_barrier.h"

struct BreakableBarrier *BreakableBarrier_SetPosition(struct BreakableBarrier *, struct SceneVec *,
                                                      int);
void sub_08012D78(u32, u32, struct BreakableBarrier *);
void sub_08012D08(struct BreakableBarrier *, u8);

int BreakableBarrier_Initialize(struct BreakableBarrierManager *m, struct BreakableBarrier *p,
                                unsigned i)
{
    struct BarrierExtent size, offset;
    int active = 0;
    void *resource;
    struct SceneCollider *c;
    if (i < m->count)
        active = 1;
    resource = p->resource;
    if (!AnimData_Load(resource, 0x9929))
        return -1;
    Sprite_Init(p, resource, 0);
    if (!active)
        p->sprite.flags |= 1;
    if (m->direction[i])
        p->sprite.flags |= 4;
    p->sprite.priority = 2;
    BreakableBarrier_SetPosition(p, &m->positions[i], m->direction[i]);
    c = &p->collider;
    if (m->direction[i]) {
        size.x = 128;
        size.y = 128;
        size.z = 110;
        offset.x = 32;
        offset.y = 128;
        offset.z = 0;
    } else {
        size.x = 110;
        size.y = 128;
        size.z = 128;
        offset.x = 0;
        offset.y = 128;
        offset.z = 32;
    }
    Collider_Init(c, 0, 0x4001, 0, 16, &size, &offset);
    if (!active)
        c->state |= 4;
    Collider_SetPosition(c, &p->sprite.position, 0);
    Collider_SetCallback(c, sub_08012D78, p);
    sub_0821FF7C(c, 0, 0, 0);
    Collider_Refresh(c);
    if (active)
        sub_08012D08(p, 1);
    else
        sub_08012D08(p, 0);
    p->health = m->health;
    return 0;
}
