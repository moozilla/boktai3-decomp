#include "global.h"
struct MotionPosition {
    s16 x, y, z, pad;
};
struct HresvelgrMotion {
    u8 pad0[0x18];
    u32 flags18;
    u8 pad1C[0x34 - 0x1C];
    struct MotionPosition position;
    u8 pad3C[0x66 - 0x3C];
    u16 colliderFlags;
    u8 pad68[0xB4 - 0x68];
    struct MotionPosition base;
    u8 padBC[0xC4 - 0xBC];
    struct MotionPosition offset;
    u8 padCC[0x138 - 0xCC];
    u16 resetState[2];
};
static inline void Sum(struct MotionPosition *out, struct MotionPosition *a, struct MotionPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
void Hresvelgr_ResetMotionState(struct HresvelgrMotion *p)
{
    if (p->resetState[1] == 0) {
        p->colliderFlags |= 4;
        p->flags18 &= ~1;
        p->resetState[1]++;
    }
    Sum(&p->position, &p->offset, &p->base);
    p->resetState[0] = 1;
    p->resetState[1] = 0;
}
