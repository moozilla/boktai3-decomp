#include "global.h"
struct ContactPosition {
    s16 x, y, z, pad;
};
void DirectionBurst_Emit(const void *, u32, u32, u32);
void sub_0819F4D8(const struct ContactPosition *origin, s32 direction, s32 spacing, u32 kind,
                  u32 parameter)
{
    struct ContactPosition position = *origin;
    s16 *coordinate = &position.z;
    s32 i, angle;
    if (direction)
        coordinate = &position.x;
    *coordinate -= spacing;
    switch (direction) {
        case 0:
            for (angle = -16, i = 2; i >= 0; i--) {
                DirectionBurst_Emit(&position, angle, kind, parameter);
                *coordinate += spacing;
                angle += 16;
            }
            break;
        case 1:
            for (i = 2, angle = 80; i >= 0; i--) {
                DirectionBurst_Emit(&position, angle, kind, parameter);
                *coordinate += spacing;
                angle -= 16;
            }
            break;
    }
}
