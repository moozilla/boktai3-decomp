
#ifdef NONMATCHING
// fuzzy: 96.4%
// foundry draft (worker best 96.4%); what differs is mostly register allocation and instruction order
#define Hresvelgr_ResetMotionState sub_081A4F78
#include "global.h"
struct HresvelgrPosition {
    s16 x, y, z, pad;
};
struct HresvelgrMotionFlags {
    u8 unknown0[6];
    u16 flags;
};
struct HresvelgrMotion {
    u8 unknown0[0x18];
    u32 flags;
    u8 unknown1C[0x18];
    struct HresvelgrPosition position;
    u8 unknown3C[0x24];
    struct HresvelgrMotionFlags motion;
    u8 unknown68[0x4c];
    struct HresvelgrPosition offset;
    u8 unknownBC[8];
    struct HresvelgrPosition origin;
    u8 unknownCC[0x6c];
    u16 state, stateFrame;
};
static inline void add(struct HresvelgrPosition *d, struct HresvelgrPosition *b,
                       struct HresvelgrPosition *a)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}
static inline void setFlags(struct HresvelgrMotionFlags *p, u32 mask)
{
    p->flags = mask | p->flags;
}
static inline void setState(struct HresvelgrMotion *p, u32 state)
{
    p->state = state;
    p->stateFrame = 0;
}
void sub_081A4F78(struct HresvelgrMotion *p)
{
    if (!p->stateFrame) {
        setFlags(&p->motion, 4);
        p->flags &= ~1;
        ++p->stateFrame;
    }
    add(&p->position, &p->origin, &p->offset);
    setState(p, 1);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A4F78);
#endif
