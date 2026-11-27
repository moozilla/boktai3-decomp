#include "global.h"
#include "scene/collision/contact_position.h"
#include "bosses/hresvelgr/motion_entry_layout.h"
#include "bosses/vanargand/vanargand_parts.h"
void HresvelgrMotion_UpdatePosition(struct MotionEntry *);
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
            Vanargand_Difference(&p->delta, pos, old);
        }
        p->mode = enabled;
    }
}
