#include "global.h"
#include "ram.h"
void Camera_Reset(void);

void sub_0822624C(void)
{
    u8 *camera = gActiveCamera;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_Reset();
    }
}
