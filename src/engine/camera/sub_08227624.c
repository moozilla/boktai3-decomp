#include "engine/camera/camera_tracking.h"
#include "bosses/vanargand/vanargand_parts.h"
struct CameraPackedPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
u16 Sqrt(u32);
s32 Div(s32, s32);
void Camera_SetPosition(struct CameraPackedPosition *);
void CameraTracking_UpdatePosition(struct CameraTracking *tracking)
{
    struct CameraPosition *first = tracking->first;
    struct CameraPosition *second = tracking->second;
    s32 x = second->x - first->x;
    s32 z = second->z - first->z;
    s32 value = Div(Sqrt(x * x + z * z), tracking->divisor);
    s32 delta = value - tracking->scale;
    s32 magnitude = delta;
    s32 factor;
    struct CameraPackedPosition position;
    if (delta < 0)
        magnitude = -delta;
    if (magnitude < tracking->scaleStep)
        tracking->scale = value;
    else if (delta > 0)
        tracking->scale += tracking->scaleStep;
    else if (delta < 0)
        tracking->scale -= tracking->scaleStep;
    if (tracking->scale < tracking->minimum)
        tracking->scale = tracking->minimum;
    if (tracking->scale > tracking->maximum)
        tracking->scale = tracking->maximum;
    factor = Vanargand_TruncateShift(tracking->scale * tracking->blend, 7);
    position.x = first->x + Vanargand_TruncateShift((second->x - first->x) * factor, 8);
    if (Vanargand_MaskedFlags(&tracking->flags, 1))
        position.y = first->y + 150;
    else
        position.y = first->y + Vanargand_TruncateShift((second->y - first->y) * factor, 8);
    position.z = first->z + Vanargand_TruncateShift((second->z - first->z) * factor, 8);
    Camera_SetPosition(&position);
}
