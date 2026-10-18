#include "global.h"
#define MotionEntry_SetEnabled     sub_081A4834
#define MotionEntry_UpdatePosition sub_081A4064
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
void sub_081A4064(struct MotionEntry *);
static inline void Difference(struct MotionPosition *out, struct MotionPosition *a,
                              struct MotionPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
void sub_081A4834(u8 *raw, u32 enabled)
{
    struct MotionPosition previous;
    if (raw) {
        struct MotionEntry *p = (struct MotionEntry *)raw;
        if (!raw[0x59] && enabled == 1) {
            struct MotionPosition *old = &previous;
            struct MotionPosition *pos = &p->position;
            *old = *pos;
            sub_081A4064(p);
            Difference(&p->delta, pos, old);
        }
        p->enabled = enabled;
    }
}
