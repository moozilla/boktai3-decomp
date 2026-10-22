#include "global.h"
extern u8 *gUnk_03005420;
void Camera_ReturnToDefault(void *);

void sub_082264B0(void *target)
{
    u8 *camera = gUnk_03005420;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_ReturnToDefault(target);
    }
}
