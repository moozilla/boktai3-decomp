#include "global.h"
#include "scene_panel_schedule.h"

void DirectionBurst_EmitStaggeredLine(const struct ContactPosition *origin, s32 direction, s32 spacing, u32 kind,
                  u32 parameter)
{
    struct ContactPosition position = *origin;
    s16 *coordinate = &position.z;
    s32 i;

    if (direction)
        coordinate = &position.x;
    *coordinate -= (spacing * 5) >> 1;
    for (i = 0; i < 6; i++) {
        if (!direction)
            DirectionBurst_Emit(&position, (i >> 1) * 16 - 32, kind, parameter);
        else {
            s32 angle = direction * 64;
            s32 offset = (i >> 1) * 16 - 32;
            DirectionBurst_Emit(&position, angle - offset, kind, parameter);
        }
        *coordinate += spacing;
    }
}
