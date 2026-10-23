#include "global.h"

#define MotionEntry_UpdatePosition sub_081A4064
struct MotionVector {
    s16 x, y, z, pad;
};
struct MotionEntry {
    u8 unknown0[0x1C];
    struct MotionVector position;
    u8 unknown24[0x28];
    struct MotionVector *origin;
    u16 lifetime, frame, radius, height;
    u8 unknown58[2];
    u16 angularStep;
};
extern const s16 gUnk_086149C4[];
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct MotionVector *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void add(struct MotionVector *out, struct MotionVector *a, struct MotionVector *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
void MotionEntry_UpdatePosition(struct MotionEntry *entry)
{
    struct MotionVector offset;
    s32 angle = (entry->frame * entry->angularStep) & 255;
    polar(&offset, angle, entry->radius);
    offset.y += entry->height;
    add(&entry->position, &offset, entry->origin);
}
