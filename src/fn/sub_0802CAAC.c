#include "global.h"
// Tentative: Select the first party target within angular and distance limits.
struct Vec {
    s16 x, y, z, pad;
};
struct Object {
    u8 pad0[9];
    u8 angle;
    u8 pada[2];
    struct Vec pos;
    u8 pad14[0x124];
    u32 range;
    u8 arc, pad13d[3];
    u32 distance;
    u8 delta;
    s8 target;
    u8 count, slot;
};
void *ActorPool_GetManager(void);
struct Object *Actor_GetPartyTarget(int);
int Math_Atan2(int, int);
int Actor_FindTargetInFacingArc(struct Object *p)
{
    int i;
    if (!ActorPool_GetManager())
        return -1;
    for (i = 0; i < (s8)p->count; i++) {
        struct Object *t = Actor_GetPartyTarget(i);
        if (t) {
            struct Vec *src = &p->pos;
            struct Vec *dst = &t->pos;
            int angle = Math_Atan2(dst->x - src->x, dst->z - src->z);
            int facing = p->angle;
            int signedAngle = (s8)angle;
            u8 diff;
            int magnitude;
            diff = facing - signedAngle;
            magnitude = (s8)diff;
            if (magnitude < 0)
                magnitude = -magnitude;
            if (magnitude <= p->arc) {
                int dx = dst->x - src->x;
                int dz = dst->z - src->z;
                u32 distance = dx * dx + dz * dz;
                if (distance <= p->range) {
                    p->delta = diff;
                    p->distance = distance;
                    return i;
                }
            }
        }
    }
    return -1;
}
