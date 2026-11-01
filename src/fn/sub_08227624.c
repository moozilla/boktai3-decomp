#include "camera_tracking.h"
struct CameraPackedPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
u16 Sqrt(u32);
s32 Div(s32, s32);
void Camera_SetPosition(struct CameraPackedPosition *);
static inline u32 Test(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline s32 Scale(s32 v, u32 shift)
{
    return v >= 0 ? v >> shift : -((-v) >> shift);
}
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
    factor = Scale(tracking->scale * tracking->blend, 7);
    position.x = first->x + Scale((second->x - first->x) * factor, 8);
    if (Test(&tracking->flags, 1))
        position.y = first->y + 150;
    else
        position.y = first->y + Scale((second->y - first->y) * factor, 8);
    position.z = first->z + Scale((second->z - first->z) * factor, 8);
    Camera_SetPosition(&position);
}
