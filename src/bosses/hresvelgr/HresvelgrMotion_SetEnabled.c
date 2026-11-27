#include "global.h"
#include "scene/collision/contact_position.h"
#include "bosses/hresvelgr/motion_entry_layout.h"
void HresvelgrMotion_UpdatePosition(struct MotionEntry *);
static inline void Difference(struct ContactPosition *out, struct ContactPosition *a,
                              struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
void HresvelgrMotion_SetEnabled(u8 *raw, u32 enabled)
{
    struct ContactPosition previous;
    if (raw) {
        struct MotionEntry *p = (struct MotionEntry *)raw;
        if (!raw[0x59] && enabled == 1) {
            struct ContactPosition *old = &previous;
            struct ContactPosition *pos = &p->position;
            *old = *pos;
            HresvelgrMotion_UpdatePosition(p);
            Difference(&p->delta, pos, old);
        }
        p->mode = enabled;
    }
}
