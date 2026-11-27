#include "global.h"
#include "scene/collision/contact_position.h"
#include "bosses/hresvelgr/motion_entry_layout.h"

extern const s16 gUnk_086149C4[];
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void add(struct ContactPosition *out, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
void HresvelgrMotion_UpdatePosition(struct MotionEntry *entry)
{
    struct ContactPosition offset;
    s32 angle = (entry->frame * entry->angularStep) & 255;
    polar(&offset, angle, entry->radius);
    offset.y += entry->height;
    add(&entry->position, &offset, entry->origin);
}
