#include "global.h"
extern u8 *gUnk_03005420;
void Camera_Reset(void);

void sub_0822624C(void)
{
    u8 *camera = gUnk_03005420;
    if (camera != NULL) {
        *(u16 *)(camera + 0x1e) = 0;
        Camera_Reset();
    }
}
