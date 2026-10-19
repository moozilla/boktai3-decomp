#include "global.h"
#include "scene_panel_schedule.h"
#ifdef NONMATCHING
// fuzzy: 55.4%
// foundry draft (worker best 97.6%); what differs is mostly register allocation and instruction order
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
    else
        DirectionBurst_Emit(&position, direction * 64 - (index * 16 - 32), kind, parameter);
}
#else
INCLUDE_ASM("asm/nonmatching", DirectionBurst_EmitOffset);
#endif
