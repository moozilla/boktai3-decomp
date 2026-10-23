#include "global.h"
#include "scene_panel_schedule.h"
// MATCHING_CONCESSION: register-pinning
#define DirectionBurst_EmitOffset sub_0819F55C
void DirectionBurst_EmitOffset(const struct ContactPosition *origin, s32 index, s32 direction,
                               s32 spacing, u32 kind, u32 parameter)
{
    struct ContactPosition position = *origin;
    s16 *coordinate = &position.z;
    if (direction)
        coordinate = &position.x;
    *coordinate = *coordinate - spacing * 2 + spacing * index;
    if (!direction)
        DirectionBurst_Emit(&position, index * 16 - 32, kind, parameter);
    else {
        s32 angle = direction * 64;
        s32 offset = index * 16 - 32;
        asm("" : "+r"(offset));
        DirectionBurst_Emit(&position, angle - offset, kind, parameter);
    }
}
