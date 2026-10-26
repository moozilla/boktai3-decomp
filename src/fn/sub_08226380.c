#include "global.h"
#include "ram.h"
struct Camera {
    u8 pad0[0x1e];
    u16 tracking;
};
void Camera_PanTo(s32, void *);
void sub_08226380(s32 speed, void *target)
{
    struct Camera *camera = gActiveCamera;
    if (camera != NULL) {
        camera->tracking = 0;
        Camera_PanTo(speed, target);
    }
}
