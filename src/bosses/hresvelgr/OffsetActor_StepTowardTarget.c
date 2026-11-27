#include "global.h"
#include "scene/collision/contact_position.h"
#include "bosses/vanargand/vanargand_parts.h"
struct OffsetActor {
    u8 pad0[0x54];
    struct ContactPosition position, motion;
    u8 pad64[0x39C - 0x64];
    struct ContactPosition target;
};
s32 Math_StepTowardTarget(s32, s32, s32);
void OffsetActor_StepTowardTarget(struct OffsetActor *p)
{
    struct ContactPosition v;
    struct ContactPosition *current = &p->position, *target = &p->target;
    s32 x = Math_StepTowardTarget(current->x, target->x, 12);
    s32 y = Math_StepTowardTarget(current->y, target->y, 12);
    s32 z = Math_StepTowardTarget(current->z, target->z, 12);
    struct ContactPosition *out = &v;
    out->x = x;
    out->y = y;
    out->z = z;
    Vanargand_Difference(&p->motion, &v, &p->position);
}
