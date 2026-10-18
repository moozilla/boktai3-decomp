#include "global.h"
struct MotionPosition {
    s16 x, y, z, pad;
};
struct MotionEntry {
    u8 unknown0[0x1c];
    struct MotionPosition position;
    u8 unknown24[0x20];
    struct MotionPosition delta;
    u8 unknown4C[0xD];
    u8 enabled;
};
void MotionEntry_UpdatePosition(struct MotionEntry *);
static inline void Difference(struct MotionPosition *out, struct MotionPosition *a,
                              struct MotionPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
void MotionEntry_SetEnabled(u8 *raw, u32 enabled)
{
    struct MotionPosition previous;
    if (raw) {
        struct MotionEntry *p = (struct MotionEntry *)raw;
        if (!raw[0x59] && enabled == 1) {
            struct MotionPosition *old = &previous;
            struct MotionPosition *pos = &p->position;
            *old = *pos;
            MotionEntry_UpdatePosition(p);
            Difference(&p->delta, pos, old);
        }
        p->enabled = enabled;
    }
}
