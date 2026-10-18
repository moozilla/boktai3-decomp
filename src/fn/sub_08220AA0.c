#include "global.h"

extern u8 *gUnk_03001688;

void Collider_OnManagerDestroy(void)
{
    gUnk_03001688 = 0;
}
