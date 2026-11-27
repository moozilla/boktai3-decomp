#include "global.h"
#include "scene/collision/contact_position.h"
struct HresvelgrCollider {
    u8 pad0[6];
    u16 flags;
};
struct HresvelgrMotion {
    u8 pad0[0x18];
    u32 flags18;
    u8 pad1C[0x34 - 0x1C];
    struct ContactPosition position;
    u8 pad3C[0x60 - 0x3C];
    struct HresvelgrCollider collider;
    u8 pad68[0xB4 - 0x68];
    struct ContactPosition base;
    u8 padBC[0xC4 - 0xBC];
    struct ContactPosition offset;
    u8 padCC[0x138 - 0xCC];
    u16 resetState[2];
};
static inline void Sum(struct ContactPosition *out, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
void Hresvelgr_ResetMotionState(struct HresvelgrMotion *p)
{
    int one;
    if (p->resetState[1] == 0) {
        struct HresvelgrCollider *c = &p->collider;
        c->flags |= 4;
        p->flags18 &= ~1;
        p->resetState[1]++;
    }
    Sum(&p->position, &p->base, &p->offset);
    one = 1;
    p->resetState[0] = 1;
    p->resetState[one] = 0;
}
