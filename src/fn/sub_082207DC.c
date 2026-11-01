// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Box_Overlaps (Box_Overlaps) inlined; the stack box address is kept in a register
// (pinned: agbcc otherwise folds it into sp-relative adds).

struct HitPoint {
    s32 a, b;
};
struct Collider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0xC];
    s16 offset[4];
    s16 ext[3];
    s16 pad;
    s16 pos[3];
    u8 pad2A[2];
    struct HitPoint hit;
    u8 pad34[0x54 - 0x34];
};
void sub_082203A8(void *a, void *b, struct Collider *o);
u32 Collider_TestSegmentBounds(void *start, void *end, struct Collider *collider, void *output);

static inline s32 axis(struct Collider *a, struct Collider *b, s32 i)
{
    s32 x = a->ext[i];
    s32 y = b->ext[i];
    s16 e = x + y;
    s32 hit = 0;
    s32 pa = a->pos[i];
    if (pa - e <= b->pos[i] && b->pos[i] - e <= pa)
        hit = 1;
    return hit;
}

static inline s32 Box_Overlaps(struct Collider *a, struct Collider *b)
{
    s32 result = 0;
    if (axis(a, b, 0) && axis(a, b, 1) && axis(a, b, 2))
        result = 1;
    return result;
}

u32 sub_082207DC(struct Collider *a, struct Collider *b)
{
    struct Collider box;
    struct HitPoint out;
    struct Collider *bp;

    sub_082203A8(a->pos, a->offset, &box);
    bp = &box;
    DEBUG_NOP();
    if ((!Box_Overlaps(bp, b)) || (!Collider_TestSegmentBounds(a->pos, a->offset, b, &out))) {
        return 0;
    }
    if (!(b->flags & 2)) {
        b->hit = out;
        a->hit = out;
    } else {
        b->hit = out;
    }
    return 1;
}
