#include "global.h"
#include "ram.h"
void Camera_ReturnToDefault(void *);

void sub_082264B0(void *target)
{
    u8 *camera = gActiveCamera;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_ReturnToDefault(target);
    }
}
