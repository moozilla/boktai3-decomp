#include "global.h"

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

static inline s32 Box_OverlapsAxis(struct Collider *a, struct Collider *b, s32 i)
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
    if (Box_OverlapsAxis(a, b, 0) && Box_OverlapsAxis(a, b, 1) && Box_OverlapsAxis(a, b, 2))
        result = 1;
    return result;
}

// Replace the segment candidate with its enclosing box for the broad-phase test.
static inline void BuildSegmentBounds(struct Collider **candidate, struct Collider *storage)
{
    struct Collider *segment = *candidate;

    sub_082203A8(segment->pos, segment->offset, storage);
    *candidate = storage;
}

u32 sub_082207DC(struct Collider *a, struct Collider *b)
{
    struct Collider box;
    struct HitPoint hit;
    struct Collider *bounds = a;

    BuildSegmentBounds(&bounds, &box);
    if (!Box_Overlaps(bounds, b) || !Collider_TestSegmentBounds(a->pos, a->offset, b, &hit))
        return 0;

    if (!(b->flags & 2)) {
        b->hit = hit;
        a->hit = hit;
    } else {
        b->hit = hit;
    }
    return 1;
}
