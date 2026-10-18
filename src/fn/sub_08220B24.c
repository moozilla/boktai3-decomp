#define Collision_AccumulateNearbyResponse sub_08220B24
#include "global.h"
struct CollisionEntry {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x36];
    u16 response;
    u16 unknown40;
    u8 layer;
    u8 responseFlags;
};
void sub_08220B24(struct CollisionEntry *a, struct CollisionEntry *b, s32 amount)
{
    if ((b->flags & 0x100) && (a->flags & 0x100)) {
        s32 delta = b->layer - a->layer;
        if (delta < 0)
            delta = -delta;
        if (delta <= 32) {
            b->response += (amount << 4) >> 6;
            b->responseFlags |= 8;
        }
    }
}
