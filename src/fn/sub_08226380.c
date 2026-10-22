#include "global.h"
struct Camera {
    u8 pad0[0x1e];
    u16 tracking;
};
extern struct Camera *gUnk_03005420;
void Camera_PanTo(s32, void *);
void sub_08226380(s32 speed, void *target)
{
    struct Camera *camera = gUnk_03005420;
    if (camera != NULL) {
        camera->tracking = 0;
        Camera_PanTo(speed, target);
    }
}
